/*
 * Copyright (c) 2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED0_N DT_ALIAS(led0)
#define LED1_N DT_ALIAS(led1)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_N, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_N, gpios);

uint32_t _blink_delay_ms = 250;
struct gpio_dt_spec *_selected_led;
K_MUTEX_DEFINE(led_mutex);

void led_set_selected(uint8_t sel)
{
	if (sel) {
		_selected_led = (void *)&led1;
	} else {
		_selected_led = (void *)&led0;
	}
}

extern void led_blink(void) {
	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);

	led_set_selected(1);

	while (1) {
		if (k_mutex_lock(&led_mutex, K_MSEC(5)) == 0) {
			gpio_pin_toggle_dt(_selected_led);
			k_mutex_unlock(&led_mutex);
		}
		k_sleep(K_MSEC(_blink_delay_ms));
	}
}
K_THREAD_DEFINE(blink_thread_id, 1024, led_blink, NULL, NULL, NULL, 5, 0, 0);

void led_wake_thread(void)
{
	if (k_mutex_lock(&led_mutex, K_MSEC(5)) == 0) {
		gpio_pin_set_dt(&led0, 0);
		gpio_pin_set_dt(&led1, 0);
		k_mutex_unlock(&led_mutex);
	}
	k_wakeup(blink_thread_id);
}

void led_set_delay(uint32_t delay)
{
	_blink_delay_ms = delay;
	led_wake_thread();
}

