#include "spi_devices.h"
#include "pn532nfc.h"

#define OP_READ_STATUS 0x02
#define OP_WRITE_DATA 0x01
#define OP_READ_DATA 0x03

#define WRITE 0x80
#define WRITE_SUB 0xC0
#define READ 0x00
#define READ_SUB 0x40
#define RW_SUB_EXT 0x80
#define NO_SUB 0xFF

spi_device_handle_t spi_pn532;

void pn532_spi_pre_cb(spi_transaction_t *trans) {
    ESP_ERROR_CHECK(gpio_set_level(PN532_CS_PIN, 0));
    esp_rom_delay_us(1000);
}

void pn532_spi_post_cb(spi_transaction_t *trans) {
    ESP_ERROR_CHECK(gpio_set_level(PN532_CS_PIN, 1));
    esp_rom_delay_us(1000);
}

void spi_devices_init(void) {
    // Bus GPIO
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = ((1ULL) << SPI_MISO_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((1ULL) << SPI_MOSI_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((1ULL) << SPI_CLK_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    
    // Init SPI bus
    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = SPI_MISO_PIN,
        .mosi_io_num = SPI_MOSI_PIN,
        .sclk_io_num = SPI_CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 1024,
        .flags = SPICOMMON_BUSFLAG_MASTER,
        .data_io_default_level = true
    };
    ret = spi_bus_initialize(SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    
    // Init PN532 CS
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((1ULL) << PN532_CS_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_set_level(PN532_CS_PIN, 1));

    // Init PN532
    spi_device_interface_config_t pn532cfg = {
        .address_bits = 0,
        .command_bits = 8,
        .dummy_bits = 0,
        .mode = 0,
        .clock_source = SPI_CLK_SRC_DEFAULT,
        .clock_speed_hz = 100000,
        .spics_io_num = -1,
        .flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_BIT_LSBFIRST,
        .queue_size = 1,
        .pre_cb = pn532_spi_pre_cb,
        .post_cb = pn532_spi_post_cb,
    };
    ret = spi_bus_add_device(SPI_HOST, &pn532cfg, &spi_pn532);
    ESP_ERROR_CHECK(ret);

    // Initial communication
    ESP_ERROR_CHECK(pn532_init());
}

esp_err_t pn532_read(uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms) {
    static uint8_t rx_buffer[1];
    TickType_t start_ticks = xTaskGetTickCount();
    TickType_t timeout_ticks = (xfer_timeout_ms > 0) ? pdMS_TO_TICKS(xfer_timeout_ms) : portMAX_DELAY;
    TickType_t elapsed_ticks = 0;
    esp_err_t result = ESP_FAIL;
    bool is_ready = false;
    while (!is_ready && elapsed_ticks < timeout_ticks) {
        result = spi_device_polling_transmit(spi_pn532,
            &(spi_transaction_t) {
                .cmd = OP_READ_STATUS,
                .rxlength = 8,
                .rx_buffer = rx_buffer,
                .user = NULL,
            });
        if (result == ESP_OK && (rx_buffer[0] & 0x01) == 0x01) {
            is_ready = true;
        }
        elapsed_ticks = xTaskGetTickCount() - start_ticks;
    }
    if (result != ESP_OK)
        return result;
    result = spi_device_polling_transmit(spi_pn532,
        &(spi_transaction_t) {
            .cmd = OP_READ_DATA,
            .rxlength = read_size * 8,
            .rx_buffer = read_buffer,
            .user = NULL,
        });
    return result;
}

esp_err_t pn532_write(const uint8_t *write_buffer, size_t write_size, int xfer_timeout_ms) {
    uint8_t frame_buffer[256];
    if (write_size > 254) {
        return ESP_ERR_INVALID_SIZE;
    }
    frame_buffer[0] = 0;
    memcpy(frame_buffer + 1, write_buffer, write_size);
    frame_buffer[write_size + 1] = 0;
    return spi_device_polling_transmit(spi_pn532,
        &(spi_transaction_t) {
            .cmd = OP_WRITE_DATA,
            .length = (write_size + 2) * 8,
            .tx_buffer = frame_buffer,
            .user = NULL,
        });
}

esp_err_t pn532_is_ready() {
    uint8_t status;
    esp_err_t result = spi_device_polling_transmit(spi_pn532,
        &(spi_transaction_t) {
            .cmd = OP_READ_STATUS,
            .rxlength = 8,
            .rx_buffer = &status,
            .user = NULL,
        });

    if (result != ESP_OK)
        return result;

    return ((status & 0x01) == 0x01) ? ESP_OK : ESP_FAIL;
}

void spi_devices_destroy(void) {
    // Destroy PN532
    ESP_ERROR_CHECK(pn532_destroy());
    ESP_ERROR_CHECK(gpio_set_level(PN532_CS_PIN, 1));
    ESP_ERROR_CHECK(spi_bus_remove_device(spi_pn532));

    // Free bus
    ESP_ERROR_CHECK(spi_bus_free(SPI_HOST));
}