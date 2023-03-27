/*
 * Copyright (c) 2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(golioth_iot, LOG_LEVEL_DBG);

#include <net/golioth/system_client.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/net/coap.h>

#include "tem_sensor.h"
#include "wifi_util.h"

static struct golioth_client *client = GOLIOTH_SYSTEM_CLIENT_GET();

static K_SEM_DEFINE(golioth_connected, 0, 1);

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
