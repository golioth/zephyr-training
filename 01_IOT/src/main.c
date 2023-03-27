/*
 * Copyright (c) 2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(golioth_iot, LOG_LEVEL_DBG);

#include <net/golioth/system_client.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/net/coap.h>

#include "led_blink.h"
#include "tem_sensor.h"
#include "wifi_util.h"

static struct golioth_client *client = GOLIOTH_SYSTEM_CLIENT_GET();

static K_SEM_DEFINE(golioth_connected, 0, 1);

#define SW0_N DT_ALIAS(sw0)
#define SW1_N DT_ALIAS(sw1)
static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET_OR(SW0_N, gpios, {0});
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET_OR(SW1_N, gpios, {0});
static struct gpio_callback button_cb_data;


static int selected_led_state_handler(struct golioth_req_rsp *rsp)
{
	if (rsp->err) {
		LOG_WRN("Failed to set selected LED state on Golioth: %d", rsp->err);
		return rsp->err;
	}

	return 0;
}

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	uint8_t led_sel;
	char sbuf[32];

	if (pins & BIT(button0.pin)) {
		led_sel = 0;
	} else if (pins & BIT(button1.pin)) {
		led_sel = 1;
	} else {
		LOG_WRN("Ignoring unknown button press even: %d", pins);
		return;
	}

	led_set_selected(led_sel);
	led_wake_thread();

	/* LED labels on board silk screen are +1 from LED Node definitions */
	snprintk(sbuf, sizeof(sbuf), "\"LED%d\"", led_sel + 1);

	int err = golioth_lightdb_set_cb(client, "selected_led",
					 GOLIOTH_CONTENT_FORMAT_APP_JSON,
					 sbuf, strlen(sbuf),
					 selected_led_state_handler, NULL);
	if (err) {
		LOG_WRN("Failed to set counter: %d", err);
		return;
	}
}

static void golioth_on_connect(struct golioth_client *client)
{
	k_sem_give(&golioth_connected);
}

static int temperature_push_handler(struct golioth_req_rsp *rsp)
{
	if (rsp->err) {
		LOG_WRN("Failed to push temperature: %d", rsp->err);
		return rsp->err;
	}

	return 0;
}

void main(void)
{
	struct sensor_value temperature;
	int counter = 0;
	int err;
	char sbuf[32];

	LOG_DBG("Start Golioth IoT");

	gpio_pin_configure_dt(&button0, GPIO_INPUT);
	gpio_pin_configure_dt(&button1, GPIO_INPUT);
	gpio_pin_interrupt_configure_dt(&button0, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_pin_interrupt_configure_dt(&button1, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&button_cb_data, button_pressed, BIT(button0.pin) | BIT(button1.pin));
	/* Buttons share the same port; this will add callback for both */
	gpio_add_callback(button0.port, &button_cb_data);

	wifi_connect();

	client->on_connect = golioth_on_connect;
	golioth_system_client_start();

	k_sem_take(&golioth_connected, K_FOREVER);

	while (true) {
		LOG_INF("Sending hello! %d", counter);

		err = golioth_send_hello(client);
		if (err) {
			LOG_WRN("Failed to send hello!");
		}

		get_temperature(&temperature);
		snprintk(sbuf, sizeof(sbuf), "%d.%06d",
				temperature.val1, temperature.val2);
		LOG_INF("Streaming Temperature to Golioth: %s", sbuf);

		err = golioth_stream_push_cb(client, "temp",
					     GOLIOTH_CONTENT_FORMAT_APP_JSON,
					     sbuf, strlen(sbuf),
					     temperature_push_handler, NULL);
		if (err) {
			LOG_WRN("Failed to push temperature: %d", err);
			return;
		}

		++counter;
		k_sleep(K_SECONDS(5));
	}
}
