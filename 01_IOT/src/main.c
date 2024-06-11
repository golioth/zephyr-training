/*
 * Copyright (c) 2023-2024 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(golioth_iot, LOG_LEVEL_DBG);

#include <golioth/client.h>
#include <golioth/lightdb_state.h>
#include <golioth/rpc.h>
#include <golioth/settings.h>
#include <golioth/stream.h>
#include <samples/common/sample_credentials.h>

#include <zcbor_encode.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/net/coap.h>

#include "led_blink.h"
#include "tem_sensor.h"
#include <network_info.h>

#ifdef CONFIG_BOARD_NRF7002DK_NRF5340_CPUAPP
#include <wifi_util.h>
#else
#include <samples/common/net_connect.h>
#endif

#define LOOP_DELAY_S_MIN 1
#define LOOP_DELAY_S_MAX 100
#define BLINK_DELAY_MS_MIN 200
#define BLINK_DELAY_MS_MAX 2000

struct golioth_client *client;
static int32_t _loop_delay_s = 5;
static k_tid_t _system_thread = 0;

static K_SEM_DEFINE(golioth_connected, 0, 1);

#define SW0_N DT_ALIAS(sw0)
#define SW1_N DT_ALIAS(sw1)
static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET_OR(SW0_N, gpios, {0});
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET_OR(SW1_N, gpios, {0});
static struct gpio_callback button_cb_data;

uint8_t _led_sel = 0;

void wake_system_thread(void)
{
	k_wakeup(_system_thread);
}

static void selected_led_state_handler(struct golioth_client *client,
				       const struct golioth_response *response,
				       const char *path,
				       void *arg)
{
	if (response->status != GOLIOTH_OK)
	{
		LOG_WRN("Failed to set selected LED state on Golioth: %d", response->status);
	}
}

void update_lightdb_state_work_handler(struct k_work *work) {
	/* LED labels on board silk screen are +1 from LED Node definitions */
	int err = golioth_lightdb_set_string_async(client,
						   "selected_led",
						   _led_sel ? "LED1" : "LED0",
						   4,
						   selected_led_state_handler,
						   NULL);
	if (err) {
		LOG_WRN("Failed to set selected_led: %d", err);
		return;
	}
}

K_WORK_DEFINE(update_state_work, update_lightdb_state_work_handler);

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	if (pins & BIT(button0.pin)) {
		_led_sel = 0;
	} else if (pins & BIT(button1.pin)) {
		_led_sel = 1;
	} else {
		LOG_WRN("Ignoring unknown button press even: %d", pins);
		return;
	}

	LOG_INF("Pins: %d", pins);

	led_set_selected(_led_sel);
	led_wake_thread();

	/*
	 * Don't call Golioth APIs from an interrupt;
	 * This worker will run on the system thread
	 */
	k_work_submit(&update_state_work);
}

static enum golioth_settings_status on_loop_delay_setting(int32_t new_value, void *arg)
{
	LOG_INF("Setting loop delay to %d s", new_value);
	_loop_delay_s = new_value;
	return GOLIOTH_SETTINGS_SUCCESS;
}

static enum golioth_settings_status on_blink_delay_setting(int32_t new_value, void *arg)
{
	led_set_delay(new_value);
	LOG_INF("Set blink delay to %d milliseconds", new_value);
	led_wake_thread();
	return GOLIOTH_SETTINGS_SUCCESS;
}

static enum golioth_rpc_status on_multiply(zcbor_state_t *request_params_array,
                                           zcbor_state_t *response_detail_map,
                                           void *callback_arg)
{
	double a, b;
	double value;
	bool ok;

	ok = zcbor_float_decode(request_params_array, &a) &&
	     zcbor_float_decode(request_params_array, &b);
	if (!ok)
	{
		LOG_ERR("Failed to decode array items");
		return GOLIOTH_RPC_INVALID_ARGUMENT;
	}

	value = a * b;

	LOG_DBG("%lf * %lf = %lf", a, b, value);

	ok = zcbor_tstr_put_lit(response_detail_map, "value") &&
	     zcbor_float64_put(response_detail_map, value);
	if (!ok)
	{
		LOG_ERR("Failed to encode value");
		return GOLIOTH_RPC_RESOURCE_EXHAUSTED;
	}

	return GOLIOTH_RPC_OK;
}

static enum golioth_rpc_status on_get_network_info(zcbor_state_t *request_params_array,
						   zcbor_state_t *response_detail_map,
						   void *callback_arg)
{
	network_info_add_to_map(response_detail_map);

	return GOLIOTH_RPC_OK;
}

static void on_client_event(struct golioth_client *client,
			    enum golioth_client_event event,
			    void *arg)
{
	bool is_connected = (event == GOLIOTH_CLIENT_EVENT_CONNECTED);
	if (is_connected)
	{
		k_sem_give(&golioth_connected);
	}
	LOG_INF("Golioth client %s", is_connected ? "connected" : "disconnected");
}

static void temperature_push_handler(struct golioth_client *client,
				     const struct golioth_response *response,
				     const char *path,
				     void *arg)
{
	if (response->status != GOLIOTH_OK)
	{
		LOG_WRN("Failed to push temperature: %d", response->status);
		return;
	}

	LOG_DBG("Temperature successfully pushed");

	return;
}

int main(void)
{
	struct sensor_value temperature;
	int counter = 0;
	int err;
	uint8_t buf[16];

	LOG_DBG("Start Golioth IoT");

	/* Get system thread id so loop delay change event can wake main */
	_system_thread = k_current_get();

	gpio_pin_configure_dt(&button0, GPIO_INPUT);
	gpio_pin_configure_dt(&button1, GPIO_INPUT);
	gpio_pin_interrupt_configure_dt(&button0, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_pin_interrupt_configure_dt(&button1, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&button_cb_data, button_pressed, BIT(button0.pin) | BIT(button1.pin));
	/* Buttons share the same port; this will add callback for both */
	gpio_add_callback(button0.port, &button_cb_data);

#ifdef CONFIG_BOARD_NRF7002DK_NRF5340_CPUAPP
	wifi_connect();
#else
	if (IS_ENABLED(CONFIG_GOLIOTH_SAMPLE_COMMON)) {
		net_connect();
	}
#endif

	const struct golioth_client_config *client_config = golioth_sample_credentials_get();

	client = golioth_client_create(client_config);
	struct golioth_rpc *rpc = golioth_rpc_init(client);
	struct golioth_settings *settings = golioth_settings_init(client);

	golioth_client_register_event_callback(client, on_client_event, NULL);
	k_sem_take(&golioth_connected, K_FOREVER);

	golioth_rpc_register(rpc, "get_network_info", on_get_network_info, NULL);
	golioth_rpc_register(rpc, "multiply", on_multiply, NULL);
	golioth_settings_register_int_with_range(settings, "LOOP_DELAY_S", LOOP_DELAY_S_MIN, LOOP_DELAY_S_MAX, on_loop_delay_setting, NULL);
	golioth_settings_register_int_with_range(settings, "BLINK_DELAY_MS", BLINK_DELAY_MS_MIN, BLINK_DELAY_MS_MAX, on_blink_delay_setting, NULL);


	while (true) {
		LOG_INF("Hello Golioth! %d", counter);

		get_temperature(&temperature);

		/* Create zcbor state variable for encoding */
		ZCBOR_STATE_E(zcbor_e_state, 1, buf, sizeof(buf), 1);

		bool ok = zcbor_map_start_encode(zcbor_e_state, 1) &&
			  zcbor_tstr_put_lit(zcbor_e_state, "temp") &&
			  zcbor_float64_put(zcbor_e_state, sensor_value_to_double(&temperature)) &&
			  zcbor_map_end_encode(zcbor_e_state, 1);

		if (ok) {
			size_t payload_length = zcbor_e_state->payload - buf;

                        LOG_INF("Streaming Temperature to Golioth: %i.%06u",
                                temperature.val1, abs(temperature.val2));

                        err = golioth_stream_set_async(client,
						       "",
						       GOLIOTH_CONTENT_TYPE_CBOR,
						       buf,
						       payload_length,
						       temperature_push_handler,
						       NULL);
                        if (err) {
                          LOG_WRN("Failed to push temperature: %d", err);
                        }

                } else {
			LOG_ERR("Error encoding CBOR packet");
		}

		++counter;

		k_sleep(K_SECONDS(_loop_delay_s));
	}
}
