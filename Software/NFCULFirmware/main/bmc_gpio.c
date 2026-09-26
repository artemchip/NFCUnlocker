#include "bmc_gpio.h"
#include "pn532nfc.h"
#include "usb_kbd.h"
#include "driver/gpio.h"

bool should_detect_nfc = false;
bool police_mode = false;

bool should_cancel = false;
bool cancelled = false;

void bmc_gpio_init(void) {
    // Mode
    police_mode = false;

    // Function button pin
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = ((1ULL) << FUNCTION_BUTTON_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    ESP_ERROR_CHECK(gpio_config(&io_conf));
}

void bmc_gpio_main_loop(void *pvParameters) {
    while (true) {
        // Check NFC card
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
        uint8_t uid_length;
        esp_err_t err = pn532_read_passive_target_id(PN532_BRTY_ISO14443A_106KBPS, uid, &uid_length, 100);
        if (err == ESP_OK && uid_length == 7 && should_detect_nfc) {
            bool matches1 = true;
            for (int i = 0; i < 7; i++) {
                if (uid[i] != MY_DESFIRE_CARD_ID_1[i] && uid[i] != MY_DESFIRE_CARD_ID_2[i]) {
                    matches1 = false;
                }
            }
            if (matches1) {
                bool matches2 = true;
                err = pn532_in_list_passive_target();
                if (err != ESP_OK) {
                    matches2 = false;
                }
                err = ntag2xx_get_model();
                if (err != ESP_OK) {
                    matches2 = false;
                }
                if (matches2) {
                    // Open, emulate keyboard
                    ESP_LOGI("Main", "NFC read!");
                    usb_emulate_password(police_mode);
                    should_detect_nfc = false;
                }
            }
        }

        // Check button press
        if (gpio_get_level(FUNCTION_BUTTON_PIN) == 0) {
            // Button pressed, wait until it is released
            int ms_passed = 0;
            while (ms_passed < 3000) {
                vTaskDelay(pdMS_TO_TICKS(100));
                ms_passed += 100;
                if (gpio_get_level(FUNCTION_BUTTON_PIN) > 0) {
                    break;
                }
            }
            ESP_LOGI("Main", "Button press!");

            // How much time has it been pressed?
            if (ms_passed >= 3000) {
                // Enter police mode, until reboot
                police_mode = true;
                should_detect_nfc = true;
            } else {
                // Set normal mode
                should_detect_nfc = true;
            }
        }
    }
    vTaskDelete(NULL);
    cancelled = true;
}

void bmc_gpio_destroy(void) {
    should_cancel = true;
    while (!cancelled) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
