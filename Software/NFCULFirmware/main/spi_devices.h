#include "bmc_constants.h"

#pragma once
void spi_devices_init(void);
esp_err_t pn532_read(uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms);
esp_err_t pn532_write(const uint8_t *write_buffer, size_t write_size, int xfer_timeout_ms);
esp_err_t pn532_is_ready();
esp_err_t dw1000_read_bytes_from_register(uint8_t cmd, uint16_t offset, uint8_t data[], uint16_t data_size);
esp_err_t dw1000_write_bytes_to_register(uint8_t cmd, uint16_t offset, uint8_t data[], uint16_t data_size);
void spi_devices_destroy(void);