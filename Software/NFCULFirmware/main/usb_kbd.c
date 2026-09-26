#include "usb_kbd.h"
#include "tinyusb.h"
#include "tinyusb_default_config.h"
#include "class/hid/hid_device.h"
#include "class/hid/hid.h"

bool suspended = false;

#define TUSB_DESC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)

const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(HID_ITF_PROTOCOL_MOUSE))
};

const char *hid_string_descriptor[5] = {
    (char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
    "TinyUSB", // 1: Manufacturer
    "TinyUSB Device", // 2: Product
    "123456", // 3: Serials, should use chip ID
    "Example HID interface", // 4: HID
};

static const uint8_t hid_configuration_descriptor[] = {
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    TUD_HID_DESCRIPTOR(0, 4, false, sizeof(hid_report_descriptor), 0x81, 16, 10),
};

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    return hid_report_descriptor;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
}

void tud_suspend_cb(bool remote_wakeup_en)
{
    //suspended = true;
}

void tud_resume_cb(void)
{
    //suspended = false;
}

void usb_emulate_password(bool police)
{
    if ((tud_mounted()) && (!suspended)) {
        // Print
        for (int i = 0; i < strlen((police) ? POLICE_PASSWORD : MY_PASSWORD); i++) {
            char curr_char = (police) ? POLICE_PASSWORD[i] : MY_PASSWORD[i];
            uint8_t keycode[6] = {HID_KEY_A};
            if (curr_char == '0') {
                keycode[0] = HID_KEY_0;
            } else if (curr_char == '1') {
                keycode[0] = HID_KEY_1;
            } else if (curr_char == '2') {
                keycode[0] = HID_KEY_2;
            } else if (curr_char == '3') {
                keycode[0] = HID_KEY_3;
            } else if (curr_char == '4') {
                keycode[0] = HID_KEY_4;
            } else if (curr_char == '5') {
                keycode[0] = HID_KEY_5;
            } else if (curr_char == '6') {
                keycode[0] = HID_KEY_6;
            } else if (curr_char == '7') {
                keycode[0] = HID_KEY_7;
            } else if (curr_char == '8') {
                keycode[0] = HID_KEY_8;
            } else if (curr_char == '9') {
                keycode[0] = HID_KEY_9;
            }
            tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, keycode);
            vTaskDelay(pdMS_TO_TICKS(35));
            tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, NULL);
            vTaskDelay(pdMS_TO_TICKS(35));
        }

        // Press Enter
        uint8_t keycode[6] = {HID_KEY_ENTER};
        tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, keycode);
        vTaskDelay(pdMS_TO_TICKS(35));
        tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, NULL);
    }
}

void usb_kbd_init(void) {
    tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
    tusb_cfg.descriptor.device = NULL;
    tusb_cfg.descriptor.full_speed_config = hid_configuration_descriptor;
    tusb_cfg.descriptor.string = hid_string_descriptor;
    tusb_cfg.descriptor.string_count = sizeof(hid_string_descriptor) / sizeof(hid_string_descriptor[0]);
    tusb_cfg.descriptor.high_speed_config = hid_configuration_descriptor;
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
}

void usb_kbd_destroy(void) {
    tinyusb_driver_uninstall();
}
