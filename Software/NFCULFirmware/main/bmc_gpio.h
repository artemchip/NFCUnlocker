#include "bmc_constants.h"
#include <cJSON.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "soc/soc_caps.h"

#pragma once
void bmc_gpio_init(void);
void bmc_gpio_pulsecnt_loop(void);
float resistive_ratio_to_temp_degrees(float ratio);
bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
void example_adc_calibration_deinit(adc_cali_handle_t handle);
void bmc_send_cmd_to_daly_bms(uint8_t cmd, uint8_t extra, cJSON** response);
void bmc_gpio_open_door(void);
void bmc_gpio_beep(void);
void bmc_gpio_main_loop(void *pvParameters);
void bmc_gpio_destroy(void);