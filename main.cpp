/*    Copyright (c) 2021 NexFarming
 *    All rights reserved
 *
 *    Author: Tasnim Zotder
 *    Git: git@github.com:NexFarming/farm-a1-temp-control.git
 */

#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

const uint LED_PIN = 25;
const uint DELAY_TIME = 1000;
const float IDEAL_TEMP_MIN = 27.0f;
const float IDEAL_TEMP_MAX = 30.0f;

// relay pins for bulb
const uint RELAY_PIN_0 = 10;  // fan 0
const uint RELAY_PIN_1 = 11;  // fan 1
const uint RELAY_PIN_2 = 12;  // fan 2
const uint RELAY_PIN_3 = 13;  // cooler 0

float get_room_temp() {
  adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);

  float temperature;
  float temp_sum = 0;
  const int loop_count = 10;

  for (size_t idx = 0; idx < loop_count; idx++) {
    uint16_t raw = adc_read();
    const float conversion_factor = 3.3f / (1 << 12);
    float voltage = raw * conversion_factor;
    temperature = 27 - (voltage - 0.706) / 0.001721;

    temp_sum += temperature;

    sleep_ms(10);
  }

  temperature = temp_sum / loop_count;

  return temperature;
}

int control_relay(int relay_0, int relay_1, int relay_2, int relay_3) {
  // heaters
  gpio_put(RELAY_PIN_0, relay_0);
  gpio_put(RELAY_PIN_1, relay_1);
  gpio_put(RELAY_PIN_2, relay_2);

  // cooler(s)
  gpio_put(RELAY_PIN_3, relay_3);

  return 0;
}

int control_bulb(float temp) {
  printf("Temp: %f\n", get_room_temp());

  if (temp >= IDEAL_TEMP_MIN && temp <= IDEAL_TEMP_MAX) {
    printf("status: in the range!\n");

    control_relay(0, 0, 0, 0);
    gpio_put(LED_PIN, 1);
  } else if (temp < IDEAL_TEMP_MIN && (IDEAL_TEMP_MIN - temp) >= 2) {
    printf("status: below the range (X)\n");

    control_relay(1, 1, 1, 0);

    for (size_t i = 0; i < DELAY_TIME / 120 + 1; i++) {
      gpio_put(LED_PIN, 0);
      sleep_ms(120 / 2);

      gpio_put(LED_PIN, 1);
      sleep_ms(120 / 2);
    }
  } else if (temp < IDEAL_TEMP_MIN && (IDEAL_TEMP_MIN - temp) < 2) {
    printf("status: below the range\n");

    control_relay(1, 1, 0, 0);

    for (size_t i = 0; i < DELAY_TIME / 250 + 1; i++) {
      gpio_put(LED_PIN, 0);
      sleep_ms(250 / 2);

      gpio_put(LED_PIN, 1);
      sleep_ms(250 / 2);
    }
  } else if (temp > IDEAL_TEMP_MAX) {
    printf("status: above the range\n");
    control_relay(0, 0, 0, 1);
  }

  return 0;
}

int main() {
  stdio_init_all();
  printf("Farm Temperature Control\n");

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  // initialize relay pins
  gpio_init(RELAY_PIN_0);
  gpio_init(RELAY_PIN_1);
  gpio_init(RELAY_PIN_2);
  gpio_init(RELAY_PIN_3);

  gpio_set_dir(RELAY_PIN_0, GPIO_OUT);
  gpio_set_dir(RELAY_PIN_1, GPIO_OUT);
  gpio_set_dir(RELAY_PIN_2, GPIO_OUT);
  gpio_set_dir(RELAY_PIN_3, GPIO_OUT);

  while (1) {
    control_bulb(get_room_temp());
    sleep_ms(DELAY_TIME);
  }
}