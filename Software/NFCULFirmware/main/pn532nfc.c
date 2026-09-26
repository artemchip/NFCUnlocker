#include "pn532nfc.h"
#include "spi_devices.h"

//#define PN532DEBUG 0
//#define MIFAREDEBUG 0
const char* TAG = "PN532";
#define PN532_PREAMBLE (0x00)
#define PN532_STARTCODE1 (0x00)
#define PN532_STARTCODE2 (0xFF)
#define PN532_POSTAMBLE (0x00)
#define PN532_HOST_TO_PN532 (0xD4)
#define PN532_PN532TOHOST (0xD5)
#define PN532_WRITE_TIMEOUT 100  // in ms
#define PN532_READ_TIMEOUT 100  // in ms
#define PN532_READY_WAIT_TIMEOUT 1000 // in ms
static const uint8_t ACK_FRAME[]  = { 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00 };
const uint8_t pn532response_firmwarevers[] = {0x00, 0xFF, 0x06, 0xFA, 0xD5, 0x03};
static uint8_t pn532_inListedTag;  // Tg number of inlisted tag.
#define PN532_COMMAND_BUFFER_LEN 64
uint8_t pn532_packetbuffer[PN532_COMMAND_BUFFER_LEN];
#define PN532_PREAMBLE (0x00)
#define PN532_STARTCODE1 (0x00)
#define PN532_STARTCODE2 (0xFF)
#define PN532_POSTAMBLE (0x00)
#define PN532_HOSTTOPN532 (0xD4)
#define PN532_PN532TOHOST (0xD5)
#define PN532_COMMAND_DIAGNOSE (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION (0x02)
#define PN532_COMMAND_GETGENERALSTATUS (0x04)
#define PN532_COMMAND_READREGISTER (0x06)
#define PN532_COMMAND_WRITEREGISTER (0x08)
#define PN532_COMMAND_READGPIO (0x0C)
#define PN532_COMMAND_WRITEGPIO (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE (0x10)
#define PN532_COMMAND_SETPARAMETERS (0x12)
#define PN532_COMMAND_SAMCONFIGURATION (0x14)
#define PN532_COMMAND_POWERDOWN (0x16)
#define PN532_COMMAND_RFCONFIGURATION (0x32)
#define PN532_COMMAND_RFREGULATIONTEST (0x58)
#define PN532_COMMAND_INJUMPFORDEP (0x56)
#define PN532_COMMAND_INJUMPFORPSL (0x46)
#define PN532_COMMAND_INLISTPASSIVETARGET (0x4A)
#define PN532_COMMAND_INATR (0x50)
#define PN532_COMMAND_INPSL (0x4E)
#define PN532_COMMAND_INDATAEXCHANGE (0x40)
#define PN532_COMMAND_INCOMMUNICATETHRU (0x42)
#define PN532_COMMAND_INDESELECT (0x44)
#define PN532_COMMAND_INRELEASE (0x52)
#define PN532_COMMAND_INSELECT (0x54)
#define PN532_COMMAND_INAUTOPOLL (0x60)
#define PN532_COMMAND_TGINITASTARGET (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES (0x92)
#define PN532_COMMAND_TGGETDATA (0x86)
#define PN532_COMMAND_TGSETDATA (0x8E)
#define PN532_COMMAND_TGSETMETADATA (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS (0x8A)
#define PN532_RESPONSE_INDATAEXCHANGE (0x41)
#define PN532_RESPONSE_INLISTPASSIVETARGET (0x4B)
#define PN532_SPI_STATREAD (0x02)
#define PN532_SPI_DATAWRITE (0x01)
#define PN532_SPI_DATAREAD (0x03)
#define PN532_SPI_READY (0x01)
#define PN532_I2C_RAW_ADDRESS (0x24)
#define PN532_I2C_ADDRESS (0x48)
#define PN532_I2C_READ_ADDRESS (0x49)
#define PN532_I2C_READBIT (0x01)
#define PN532_I2C_BUSY (0x00)
#define PN532_I2C_READY (0x01)
#define PN532_I2C_READYTIMEOUT (20)
#define MIFARE_CMD_AUTH_A (0x60)
#define MIFARE_CMD_AUTH_B (0x61)
#define MIFARE_CMD_READ (0x30)
#define MIFARE_CMD_WRITE (0xA0)
#define MIFARE_CMD_TRANSFER (0xB0)
#define MIFARE_CMD_DECREMENT (0xC0)
#define MIFARE_CMD_INCREMENT (0xC1)
#define MIFARE_CMD_STORE (0xC2)
#define MIFARE_ULTRALIGHT_CMD_WRITE (0xA2)
#define NDEF_URIPREFIX_NONE (0x00)
#define NDEF_URIPREFIX_HTTP_WWWDOT (0x01)
#define NDEF_URIPREFIX_HTTPS_WWWDOT (0x02)
#define NDEF_URIPREFIX_HTTP (0x03)
#define NDEF_URIPREFIX_HTTPS (0x04)
#define NDEF_URIPREFIX_TEL (0x05)
#define NDEF_URIPREFIX_MAILTO (0x06)
#define NDEF_URIPREFIX_FTP_ANONAT (0x07)
#define NDEF_URIPREFIX_FTP_FTPDOT (0x08)
#define NDEF_URIPREFIX_FTPS (0x09)
#define NDEF_URIPREFIX_SFTP (0x0A)
#define NDEF_URIPREFIX_SMB (0x0B)
#define NDEF_URIPREFIX_NFS (0x0C)
#define NDEF_URIPREFIX_FTP (0x0D)
#define NDEF_URIPREFIX_DAV (0x0E)
#define NDEF_URIPREFIX_NEWS (0x0F)
#define NDEF_URIPREFIX_TELNET (0x10)
#define NDEF_URIPREFIX_IMAP (0x11)
#define NDEF_URIPREFIX_RTSP (0x12)
#define NDEF_URIPREFIX_URN (0x13)
#define NDEF_URIPREFIX_POP (0x14)
#define NDEF_URIPREFIX_SIP (0x15)
#define NDEF_URIPREFIX_SIPS (0x16)
#define NDEF_URIPREFIX_TFTP (0x17)
#define NDEF_URIPREFIX_BTSPP (0x18)
#define NDEF_URIPREFIX_BTL2CAP (0x19)
#define NDEF_URIPREFIX_BTGOEP (0x1A)
#define NDEF_URIPREFIX_TCPOBEX (0x1B)
#define NDEF_URIPREFIX_IRDAOBEX (0x1C)
#define NDEF_URIPREFIX_FILE (0x1D)
#define NDEF_URIPREFIX_URN_EPC_ID (0x1E)
#define NDEF_URIPREFIX_URN_EPC_TAG (0x1F)
#define NDEF_URIPREFIX_URN_EPC_PAT (0x20)
#define NDEF_URIPREFIX_URN_EPC_RAW (0x21)
#define NDEF_URIPREFIX_URN_EPC (0x22)
#define NDEF_URIPREFIX_URN_NFC (0x23)

esp_err_t pn532_init(void) {
    uint32_t fw_v = 0;
    ESP_ERROR_CHECK(pn532_get_firmware_version(&fw_v));
    return pn532_SAM_config();
}

esp_err_t pn532_write_command(const uint8_t *cmd, uint8_t cmdlen, int timeout)
{
    uint8_t command[256];
    uint8_t checksum = PN532_HOST_TO_PN532;
    int idx = 0;
    command[idx++] = PN532_STARTCODE1;
    command[idx++] = PN532_STARTCODE2;
    command[idx++] = (cmdlen + 1);
    command[idx++] = 0x100 - (cmdlen + 1);
    command[idx++] = PN532_HOST_TO_PN532;
    uint8_t i = 0;
    for (i = 0; i < cmdlen; i++) {
        command[idx++] = cmd[i];
        checksum += cmd[i];
    }
    command[idx++] = ~checksum + 1;
#ifdef PN532DEBUG
    ESP_LOGD(TAG, "%s Sending :", __func__);
    ESP_LOG_BUFFER_HEX(TAG, command, idx);
#endif
    esp_err_t result = pn532_write(command, idx, timeout);
    if (result != ESP_OK) {
        char *resultText = NULL;
        switch (result) {
            case ESP_ERR_INVALID_ARG:
                resultText = "parameter error";
                break;
            case ESP_FAIL:
                resultText = "send command failed";
                break;
            case ESP_ERR_INVALID_STATE:
                resultText = "invalid state";
                break;
            case ESP_ERR_TIMEOUT:
                resultText = "timeout occurred";
                break;
            default:
                resultText = "unknown error";

        }
        ESP_LOGW(TAG, "%s write failed: %s!", __func__, resultText);
    }
    return result;
}

esp_err_t pn532_read_data(uint8_t *buffer, uint8_t length, int32_t timeout)
{
    uint8_t local_buffer[256];
    bzero(local_buffer, sizeof(local_buffer));
    if (timeout == 0) {
        timeout = -1;
    }
    esp_err_t res = pn532_read(local_buffer, length, timeout);
    if (res != ESP_OK) {
        return res;
    }
#ifdef PN532DEBUG
    ESP_LOGD(TAG, "Reading: ");
    ESP_LOG_BUFFER_HEX(TAG, local_buffer, length);
#endif
    memcpy(buffer, local_buffer, length);
    return ESP_OK;
}

esp_err_t pn532_wait_ready(int32_t timeout)
{
    TickType_t start_ticks = xTaskGetTickCount();
    TickType_t timeout_ticks = (timeout > 0) ? pdMS_TO_TICKS(timeout) : portMAX_DELAY;
    TickType_t elapsed_ticks = 0;
    esp_rom_delay_us(1000);
    bool is_ready = false;
    while (!is_ready && elapsed_ticks <= timeout_ticks)
    {
        is_ready = ESP_OK == pn532_is_ready();
        if (!is_ready) {
            vTaskDelay(pdMS_TO_TICKS(10));
            elapsed_ticks = xTaskGetTickCount() - start_ticks;
        }
    }
    return is_ready ? ESP_OK : ESP_ERR_TIMEOUT;
}

esp_err_t pn532_SAM_config()
{
    esp_err_t result;
    uint8_t response_buffer[16];
    static const uint8_t sam_config_frame[] = { 0x14, 0x01, 0x00, 0x00 }; // Normal mode and don't use IRQ pin
    result = pn532_send_command_wait_ack(sam_config_frame, sizeof(sam_config_frame), 1000);
    if (ESP_OK != result)
        return result;
#ifdef PN532DEBUG
    ESP_LOGD(TAG, "pn532_SAM_config(): Waiting for IRQ/ready");
#endif
    result = pn532_wait_ready(100);
    if (ESP_OK != result) {
#ifdef PN532DEBUG
        ESP_LOGD(TAG, "pn532_SAM_config(): Timeout occurred");
#endif
        return result;
    }
    result = pn532_read_data(response_buffer, 10, PN532_READ_TIMEOUT);
    if (ESP_OK != result)
        return result;
    if (response_buffer[6] != 0x15) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t pn532_send_command_wait_ack(const uint8_t *cmd, uint8_t cmd_length, int32_t timeout)
{
    esp_err_t result;
    if (cmd == NULL || cmd_length == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    result = pn532_write_command(cmd, cmd_length, timeout);
    if (result != ESP_OK) {
        return result;
    }
#ifdef PN532DEBUG
    ESP_LOGD(TAG, "pn532_send_command_wait_ack(): Waiting for PN532 IRQ/ready");
#endif
    result = pn532_wait_ready(timeout);
    if (result != ESP_OK) {
#ifdef PN532DEBUG
        if (result == ESP_ERR_TIMEOUT)
            ESP_LOGE(TAG, "pn532_send_command_wait_ack(): Timeout occurred!");
#endif
        ESP_LOGD(TAG, "pn532_wait_ready() failed with 0x%X", result);
        return result;
    }
    result = pn532_read_ack();
    if (result != ESP_OK) {
#ifdef PN532DEBUG
        ESP_LOGD(TAG, "pn532_send_command_wait_ack(): No ACK frame received!");
#endif
    }
    return result;
}

esp_err_t pn532_read_ack() {
    uint8_t ack_buffer[6];
    esp_err_t result = pn532_read_data(ack_buffer, sizeof(ACK_FRAME), PN532_READ_TIMEOUT);
    if (result != ESP_OK)
        return result;
    if (0 != memcmp(ack_buffer, ACK_FRAME, sizeof(ACK_FRAME))) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t pn532_get_firmware_version(uint32_t *fw_version)
{
    esp_err_t err;
    pn532_packetbuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;
    err = pn532_send_command_wait_ack(pn532_packetbuffer, 1, PN532_WRITE_TIMEOUT);
    if (ESP_OK != err) {
        return err;
    }
#ifdef PN532DEBUG
    ESP_LOGD(TAG, "pn532_get_firmware_version(): Waiting for IRQ/ready");
#endif
    err = pn532_wait_ready(100);
    if (ESP_OK != err) {
#ifdef PN532DEBUG
        ESP_LOGD(TAG, "pn532_get_firmware_version(): Timeout occurred");
#endif
        return err;
    }
    err = pn532_read_data(pn532_packetbuffer, 12, PN532_READ_TIMEOUT);
    if (ESP_OK != err)
        return err;
    if (0 != memcmp(pn532_packetbuffer + 1, pn532response_firmwarevers, sizeof(pn532response_firmwarevers))) {
#ifdef PN532DEBUG
        ESP_LOGD(TAG, "pn532_get_firmware_version(): get firmware response invalid!");
#endif
        return ESP_FAIL;
    }
    int offset = 7;
    *fw_version  = pn532_packetbuffer[offset++] << 24;
    *fw_version |= pn532_packetbuffer[offset++] << 16;
    *fw_version |= pn532_packetbuffer[offset++] << 8;
    *fw_version |= pn532_packetbuffer[offset];
    return ESP_OK;
}

esp_err_t pn532_set_passive_activation_retries(uint8_t maxRetries) {
    pn532_packetbuffer[0] = PN532_COMMAND_RFCONFIGURATION;
    pn532_packetbuffer[1] = 5;    // Config item 5 (MaxRetries)
    pn532_packetbuffer[2] = 0xFF; // MxRtyATR (default = 0xFF)
    pn532_packetbuffer[3] = 0x01; // MxRtyPSL (default = 0x01)
    pn532_packetbuffer[4] = maxRetries;
#ifdef MIFAREDEBUG
    ESP_LOGD(TAG, "pn532_set_passive_activation_retries(): Setting MxRtyPassiveActivation to %d", maxRetries);
#endif
    return pn532_send_command_wait_ack(pn532_packetbuffer, 5, PN532_WRITE_TIMEOUT);
}

esp_err_t pn532_read_passive_target_id(uint8_t baud_rate_and_card_type, uint8_t *uid, uint8_t *uid_length, int32_t timeout)
{
    pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
    pn532_packetbuffer[1] = 1; // currently only support one card (PN532 can handle two cards)
    pn532_packetbuffer[2] = baud_rate_and_card_type;
    esp_err_t err = pn532_send_command_wait_ack(pn532_packetbuffer, 3, PN532_WRITE_TIMEOUT);
    if (ESP_OK != err) {
#ifdef PN532DEBUG
        ESP_LOGD(TAG, "No card(s) read");
#endif
        return err;
    }

#ifdef PN532DEBUG
    ESP_LOGD(TAG, "Waiting for IRQ (indicates card presence)");
#endif
    err = pn532_wait_ready(timeout);
    if (ESP_OK != err) {
#ifdef PN532DEBUG
        ESP_LOGD(TAG, "PN532 not ready, timeout or error occurred");
#endif
        return err;
    }
    err = pn532_read_data(pn532_packetbuffer, 32, timeout);
    if (ESP_OK != err)
        return err;

    /* ISO14443A card response should be in the following format:

     byte            Description
     -------------   ------------------------------------------
     b0..6           Frame header and preamble
     b7              Number of tags Found
     b8              Tag Number (only one used in this example)
     b9..10          SENS_RES
     b11             SEL_RES
     b12             NFCID Length
     b13..NFCIDLen   NFCID                                      */

#ifdef MIFAREDEBUG
    ESP_LOGD(TAG, "Found %d tags", pn532_packetbuffer[7]);
#endif
    if (pn532_packetbuffer[7] != 1)
        return ESP_FAIL;

#ifdef MIFAREDEBUG
    uint16_t sens_res = pn532_packetbuffer[9] << 8 | pn532_packetbuffer[10];

    ESP_LOGD(TAG, "ATQA: 0x%.2X", sens_res);
    ESP_LOGD(TAG, "SAK: 0x%.2X", pn532_packetbuffer[11]);
#endif

    /* Card appears to be Mifare Classic */
    *uid_length = pn532_packetbuffer[12];
#ifdef MIFAREDEBUG
    printf("UID:");
#endif
    for (uint8_t i = 0; i < pn532_packetbuffer[12]; i++) {
        uid[i] = pn532_packetbuffer[13 + i];
#ifdef MIFAREDEBUG
        printf(" 0x%.2X", uid[i]);
#endif
    }
#ifdef MIFAREDEBUG
    printf("\n");
#endif

    return ESP_OK;
}

esp_err_t pn532_in_list_passive_target() {
    pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
    pn532_packetbuffer[1] = 1;
    pn532_packetbuffer[2] = 0;

#ifdef PN532DEBUG
    ESP_LOGD(TAG, "About to inList passive target");
#endif

    esp_err_t err = pn532_send_command_wait_ack(pn532_packetbuffer, 3, PN532_WRITE_TIMEOUT);
    if (ESP_OK != err) {
#ifdef PN532DEBUG
        ESP_LOGD(TAG, "Could not send inlistPassiveTarget message");
#endif
        return err;
    }

    err = pn532_wait_ready(600);
    if (ESP_OK != err)
        return err;

    err = pn532_read_data(pn532_packetbuffer, sizeof(pn532_packetbuffer), PN532_READ_TIMEOUT);
    if (ESP_OK != err)
        return err;

    if (pn532_packetbuffer[0] == 0 && pn532_packetbuffer[1] == 0 && pn532_packetbuffer[2] == 0xff)
    {
        uint8_t length = pn532_packetbuffer[3];
        if (0 != ((pn532_packetbuffer[4] + length) & 0xFF))
        {
#ifdef PN532DEBUG
            ESP_LOGD(TAG, "Length check invalid 0x%.2X 0x%.2X", length, pn532_packetbuffer[4]);
#endif
            return ESP_FAIL;
        }
        if (pn532_packetbuffer[5] == PN532_PN532TOHOST && pn532_packetbuffer[6] == PN532_RESPONSE_INLISTPASSIVETARGET)
        {
            if (pn532_packetbuffer[7] != 1) {
#ifdef PN532DEBUG
                ESP_LOGD(TAG, "Unhandled number of targets inlisted");
#endif
                return ESP_FAIL;
            }

            pn532_inListedTag = pn532_packetbuffer[8];

            return ESP_OK;
        } else {
#ifdef PN532DEBUG
            ESP_LOGD(TAG, "Unexpected response to inlist passive host");
#endif
            return ESP_FAIL;
        }
    }

#ifdef PN532DEBUG
    ESP_LOGD(TAG, "Preamble missing");
#endif
    return ESP_FAIL;
}

esp_err_t ntag2xx_read_page(uint8_t page, uint8_t *buffer, size_t read_len)
{
    // TAG Type       PAGES   USER START    USER STOP
    // --------       -----   ----------    ---------
    // NTAG 203       42      4             39
    // NTAG 213       45      4             39
    // NTAG 215       135     4             129
    // NTAG 216       231     4             225

    if (page >= 231 || read_len == 0) {
#ifdef MIFAREDEBUG
        ESP_LOGD(TAG, "Page value out of range");
#endif
        return ESP_ERR_INVALID_ARG;
    }

    if (read_len > 16)
        read_len = 16;

#ifdef MIFAREDEBUG
    ESP_LOGD(TAG, "Reading page %d", page);
#endif

    /* Prepare the command */
    pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
    pn532_packetbuffer[1] = 1; /* Card number */
    pn532_packetbuffer[2] = MIFARE_CMD_READ; /* Mifare Read command = 0x30 */
    pn532_packetbuffer[3] = page; /* Page Number (0..63 in most cases) */

    /* Send the command */
    esp_err_t err = pn532_send_command_wait_ack(pn532_packetbuffer, 4, PN532_WRITE_TIMEOUT);
    if (err != ESP_OK) {
#ifdef MIFAREDEBUG
        ESP_LOGD(TAG, "write failed or ACK not received for command");
#endif
        return err;
    }

#ifdef PN532DEBUG
    ESP_LOGD(TAG, "ntag2xx_ReadPage(): Waiting for IRQ/ready");
#endif
    err = pn532_wait_ready(100);
    if (ESP_OK != err) {
#ifdef PN532DEBUG
        ESP_LOGD(TAG, "ntag2xx_ReadPage(): Timeout occurred");
#endif
        return err;
    }

    /* Read the response packet */
    err = pn532_read_data(pn532_packetbuffer, 26, PN532_READ_TIMEOUT);
    if (err != ESP_OK)
        return err;

#ifdef MIFAREDEBUG
    ESP_LOGD(TAG, "Received: ");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, pn532_packetbuffer, 26, ESP_LOG_DEBUG);
#endif

    uint8_t status = pn532_packetbuffer[7];
    // check error code of status byte
    if ((status & 0x3F) == 0x00) {
        memcpy(buffer, pn532_packetbuffer + 8, read_len);
    }
    else {
#ifdef MIFAREDEBUG
        ESP_LOGD(TAG, "Status byte indicates an error: 0x%02x", pn532_packetbuffer[7]);
#endif
        return ESP_FAIL;
    }

    /* Display data for debug if requested */
#ifdef MIFAREDEBUG
    ESP_LOGD(TAG, "Page %d", page);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, buffer, 4, ESP_LOG_DEBUG);
#endif

    // Return OK signal
    return ESP_OK;
}

esp_err_t ntag2xx_get_model()
{
    uint8_t page_mem[16];
    esp_err_t err = ntag2xx_read_page(0, page_mem, sizeof(page_mem));
    if (err != ESP_OK) {
        return err;
    }
    if (page_mem[14] == 170) {
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

esp_err_t pn532_destroy(void) {
    // Don't need to do anything
    return ESP_OK;
}