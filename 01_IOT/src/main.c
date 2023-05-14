/*
 * Copyright (c) 2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(golioth_iot, LOG_LEVEL_DBG);

#include <net/golioth/settings.h>
#include <net/golioth/system_client.h>
#include <qcbor/qcbor.h>
#include <qcbor/qcbor_spiffy_decode.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/net/coap.h>

#include "led_blink.h"
#include "tem_sensor.h"

#ifdef CONFIG_BOARD_NRF7002DK_NRF5340_CPUAPP
#include "wifi_util.h"
#else
#include <samples/common/net_connect.h>
#endif

static struct golioth_client *client = GOLIOTH_SYSTEM_CLIENT_GET();
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

static int selected_led_state_handler(struct golioth_req_rsp *rsp)
{
	if (rsp->err) {
		LOG_WRN("Failed to set selected LED state on Golioth: %d", rsp->err);
		return rsp->err;
	}

	return 0;
}

void update_lightdb_state_work_handler(struct k_work *work) {
	char sbuf[32];

	/* LED labels on board silk screen are +1 from LED Node definitions */
	snprintk(sbuf, sizeof(sbuf), "\"LED%d\"", _led_sel + 1);

	int err = golioth_lightdb_set_cb(client, "selected_led",
					 GOLIOTH_CONTENT_FORMAT_APP_JSON,
					 sbuf, strlen(sbuf),
					 selected_led_state_handler, NULL);
	if (err) {
		LOG_WRN("Failed to set counter: %d", err);
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

enum golioth_settings_status on_setting(
		const char *key,
		const struct golioth_settings_value *value)
{
	LOG_DBG("Received setting: key = %s, type = %d", key, value->type);
	if (strcmp(key, "LOOP_DELAY_S") == 0) {
		/* This setting is expected to be numeric, return an error if it's not */
		if (value->type != GOLIOTH_SETTINGS_VALUE_TYPE_INT64) {
			return GOLIOTH_SETTINGS_VALUE_FORMAT_NOT_VALID;
		}

		/* This setting must be in range [1, 100], return an error if it's not */
		if (value->i64 < 1 || value->i64 > 100) {
			return GOLIOTH_SETTINGS_VALUE_OUTSIDE_RANGE;
		}

		/* Setting has passed all checks, so apply it to the loop delay */
		_loop_delay_s = (int32_t)value->i64;
		LOG_INF("Set loop delay to %d seconds", _loop_delay_s);

		wake_system_thread();

		return GOLIOTH_SETTINGS_SUCCESS;
	}

	if (strcmp(key, "BLINK_DELAY_MS") == 0) {
		/* This setting is expected to be numeric, return an error if it's not */
		if (value->type != GOLIOTH_SETTINGS_VALUE_TYPE_INT64) {
			return GOLIOTH_SETTINGS_VALUE_FORMAT_NOT_VALID;
		}

		/* This setting must be in range [100, 2000], return an error if it's not */
		if (value->i64 < 100 || value->i64 > 2000) {
			LOG_ERR("Bounding error (100 < BLINK_DELAY_MS < 10000): %lld", value->i64);
			return GOLIOTH_SETTINGS_VALUE_OUTSIDE_RANGE;
		}

		/* Setting has passed all checks, so apply it to the loop delay */
		led_set_delay((uint32_t)value->i64);
		LOG_INF("Set blink delay to %lld milliseconds", value->i64);
		led_wake_thread();

		return GOLIOTH_SETTINGS_SUCCESS;
	}

	/* If the setting is not recognized, we should return an error */
	return GOLIOTH_SETTINGS_KEY_NOT_RECOGNIZED;
}

static enum golioth_rpc_status on_get_network_info(QCBORDecodeContext *request_params_array,
						QCBOREncodeContext *response_detail_map,
						void *callback_arg)
{
	QCBORError qerr;

	qerr = QCBORDecode_GetError(request_params_array);
	if (qerr != QCBOR_SUCCESS) {
		LOG_ERR("Failed to decode array items: %d (%s)", qerr, qcbor_err_to_str(qerr));
		return GOLIOTH_RPC_INVALID_ARGUMENT;
	}

#ifdef CONFIG_BOARD_NRF7002DK_NRF5340_CPUAPP
	struct wifi_iface_status w_status = { 0 };

	cmd_wifi_status(&w_status);

	QCBOREncode_AddSZStringToMap(response_detail_map, "State", wifi_state_txt(w_status.state));

	if (w_status.state >= WIFI_STATE_ASSOCIATED) {
		uint8_t mac_string_buf[sizeof("xx:xx:xx:xx:xx:xx")];
		QCBOREncode_AddSZStringToMap(response_detail_map,
					     "Interface Mode",
					     wifi_mode_txt(w_status.iface_mode));
		QCBOREncode_AddSZStringToMap(response_detail_map,
					     "Link Mode",
					     wifi_link_mode_txt(w_status.link_mode));
		QCBOREncode_AddSZStringToMap(response_detail_map,
					     "SSID", w_status.ssid);
		QCBOREncode_AddSZStringToMap(response_detail_map,
					     "BSSID",
					     net_sprint_ll_addr_buf(w_status.bssid,
								    WIFI_MAC_ADDR_LEN,
								    mac_string_buf,
								    sizeof(mac_string_buf))
					     );
		QCBOREncode_AddSZStringToMap(response_detail_map, "Band",
					     wifi_band_txt(w_status.band));
		QCBOREncode_AddDoubleToMap(response_detail_map,
					   "Channel", w_status.channel);
		QCBOREncode_AddSZStringToMap(response_detail_map,
					     "Security",
					     wifi_security_txt(w_status.security));
		QCBOREncode_AddSZStringToMap(response_detail_map, "MFP", wifi_mfp_txt(w_status.mfp));
		QCBOREncode_AddDoubleToMap(response_detail_map, "RSSI", w_status.rssi);
	}

#else
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "No Network Info",
				     "Network reporting not implemented "
				     "for this board");

#endif

	return GOLIOTH_RPC_OK;
}

static void golioth_on_connect(struct golioth_client *client)
{
	k_sem_give(&golioth_connected);

	golioth_rpc_observe(client);
	golioth_settings_observe(client);
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
	if (IS_ENABLED(CONFIG_GOLIOTH_SAMPLES_COMMON)) {
		net_connect();
	}
#endif

	golioth_rpc_register(client, "get_network_info", on_get_network_info, NULL);
	golioth_settings_register_callback(client, on_setting);
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

		k_sleep(K_SECONDS(_loop_delay_s));
	}
}
