#include "bmc_constants.h"

#pragma once

#define PN532_BRTY_ISO14443A_106KBPS (0x00)
#define PN532_BRTY_FELICA_212KBPS (0x01)
#define PN532_BRTY_FELICA_424KBPS (0x02)
#define PN532_BRTY_ISO14443B_106KBPS (0x03)
#define PN532_BRTY_JEWEL_TAG_106KBPS (0x04)

esp_err_t pn532_init(void);
esp_err_t pn532_write_command(const uint8_t *cmd, uint8_t cmdlen, int timeout);
esp_err_t pn532_read_data(uint8_t *buffer, uint8_t length, int32_t timeout);
esp_err_t pn532_wait_ready(int32_t timeout);
esp_err_t pn532_SAM_config();
esp_err_t pn532_send_command_wait_ack(const uint8_t *cmd, uint8_t cmd_length, int32_t timeout);
esp_err_t pn532_read_ack();
esp_err_t pn532_get_firmware_version(uint32_t *fw_version);
esp_err_t pn532_read_passive_target_id(uint8_t baud_rate_and_card_type, uint8_t *uid, uint8_t *uid_length, int32_t timeout);
esp_err_t pn532_in_list_passive_target();
esp_err_t ntag2xx_read_page(uint8_t page, uint8_t *buffer, size_t read_len);
esp_err_t ntag2xx_get_model();
esp_err_t pn532_destroy(void);