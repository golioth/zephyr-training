/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(add_golioth, LOG_LEVEL_DBG);

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* Includes for connectivity */
#include <zephyr/net/coap.h>

/* Helper functions for passing credentials to Golioth client */
#include <samples/common/sample_credentials.h>

#ifdef CONFIG_BOARD_NRF7002DK_NRF5340_CPUAPP
#include <wifi_util.h>
#else
#include <samples/common/net_connect.h>
#endif

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifiers for the "led0" and "training_led" */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(training_led)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

/* Blink LED1 using a Zephyr Timer */
extern void my_timer_handler(struct k_timer *dummy) {
	gpio_pin_toggle_dt(&led1);
}

K_TIMER_DEFINE(my_timer, my_timer_handler, NULL);

/* Blink LED2 using a Zephyr Thread */
static void my_thread_handler(void *dummy1, void *dummy2, void *dummy3) {
	if (!device_is_ready(led2.port)) {
		return;
	}

	int ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

	while (1) {
		gpio_pin_toggle_dt(&led2);
		k_sleep(K_SECONDS(1));
	}
}

K_THREAD_DEFINE(my_thread, 1024,
                my_thread_handler, NULL, NULL, NULL,
                K_LOWEST_APPLICATION_THREAD_PRIO, 0, 0);

/* Main function */
int main(void)
{
	int ret;

	if (!device_is_ready(led1.port)) {
		return -EIO;
	}

	ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return -EIO;
	}

	/* Start timer-based LED blinker */
	k_timer_start(&my_timer, K_MSEC(200), K_MSEC(200));

	/* Start network connection (if necessary) */
#ifdef CONFIG_BOARD_NRF7002DK_NRF5340_CPUAPP
	wifi_connect();
#else
	if (IS_ENABLED(CONFIG_GOLIOTH_SAMPLE_COMMON)) {
		net_connect();
	}
#endif

	int counter = 0;

	while (1) {
		printk("This is the main loop: %d\n", counter);
		++counter;
		k_msleep(SLEEP_TIME_MS);
	}
}
