#include "spi_devices.h"
#include "pn532nfc.h"
#include "bmc_gpio.h"
#include "usb_kbd.h"
#include "esp_log.h"

const char* TAGMAIN = "NFCUL-Main";

void app_main(void)
{
    bmc_gpio_init();
    spi_devices_init();
    usb_kbd_init();
    bmc_gpio_main_loop(NULL);
    usb_kbd_destroy();
    spi_devices_destroy();
    bmc_gpio_destroy();
}
