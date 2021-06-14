/*    Copyright (c) 2021 NexFarming
 *    All rights reserved
 *
 *    Author: Tasnim Zotder
 *    Git: https://github.com/NexFarming/farm-a1-temp-control
 */

#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

const uint LED_PIN = 25;
const float IDEAL_ROOM_TEMP = 27.0f;

// relay pins for bulb
const uint RELAY_PIN_0 = 10;
const uint RELAY_PIN_1 = 11;
const uint RELAY_PIN_2 = 12;
const uint RELAY_PIN_3 = 13;

float get_room_temp() {
  adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);

  uint16_t raw = adc_read();
  const float conversion_factor = 3.3f / (1 << 12);
  float voltage = raw * conversion_factor;
  float temperature = 27 - (voltage - 0.706) / 0.001721;

  return temperature;
}

int control_bulb(float temp) {
  printf("Temp: %f\n", get_room_temp());

  if (temp >= IDEAL_ROOM_TEMP) {
    printf("relay: off\n");

    gpio_put(RELAY_PIN_0, 0);
    gpio_put(RELAY_PIN_1, 0);
    gpio_put(RELAY_PIN_2, 0);
    gpio_put(RELAY_PIN_3, 0);

    gpio_put(LED_PIN, 1);
  } else {
    printf("relay: on\n");

    gpio_put(RELAY_PIN_0, 1);
    gpio_put(RELAY_PIN_1, 1);
    gpio_put(RELAY_PIN_2, 1);
    gpio_put(RELAY_PIN_3, 1);

    for (size_t i = 0; i < 1000 / 250 + 1; i++) {
      gpio_put(LED_PIN, 0);
      sleep_ms(250 / 2);

      gpio_put(LED_PIN, 1);
      sleep_ms(250 / 2);
    }
  }

  printf("temp diff: %f\n\n", (temp - IDEAL_ROOM_TEMP));

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
    sleep_ms(1000);
  }
}