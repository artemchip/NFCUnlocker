#include "secrets.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#pragma once

#define SPI_HOST SPI2_HOST
#define SPI_MISO_PIN 13
#define SPI_MOSI_PIN 11
#define SPI_CLK_PIN 12
#define PN532_CS_PIN 10

#define FUNCTION_BUTTON_PIN 38
