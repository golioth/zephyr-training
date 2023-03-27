/*
 * Copyright (c) 2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(golioth_iot, LOG_LEVEL_DBG);

#include <net/golioth/system_client.h>
#include <zephyr/net/coap.h>

#include "wifi_util.h"

static struct golioth_client *client = GOLIOTH_SYSTEM_CLIENT_GET();

static K_SEM_DEFINE(golioth_connected, 0, 1);

static void golioth_on_connect(struct golioth_client *client)
{
	k_sem_give(&golioth_connected);
}

void main(void)
{
	int counter = 0;
	int err;

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
		++counter;
		k_sleep(K_SECONDS(5));
	}
}
