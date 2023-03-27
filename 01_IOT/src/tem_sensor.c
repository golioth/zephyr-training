/*
 * Copyright (c) 2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/sensor.h>
#include <zephyr/random/rand32.h>

#define TEM_GEN_DELAY_S 1
#define TEM_MIN 20
#define TEM_MAX 38
#define DELTA_LIMIT_HUNDREDTHS 50

static struct sensor_value _temperature = { .val1 = 27, .val2 = 340000 };

K_MUTEX_DEFINE(temperature_mutex);

static int32_t get_tem_large(struct sensor_value *s)
{
	return (s->val1 * 1000000) + s->val2;
}

static void set_sensor_value(int32_t tem_large, struct sensor_value *s)
{
	s->val1 = tem_large / 1000000;
	s->val2 = tem_large % 1000000;
}

static void update_temperature(struct sensor_value *tem)
{
	int32_t min = TEM_MIN * 1000000;
	int32_t max = TEM_MAX * 1000000;

	int32_t delta = (sys_rand32_get() % (DELTA_LIMIT_HUNDREDTHS + 1)) - (DELTA_LIMIT_HUNDREDTHS / 2);
	int32_t tem_large = get_tem_large(tem) + (delta * 10000);

	if (tem_large < min) {
		tem_large = min;
	} else if (tem_large > max) {
		tem_large = max;
	}

	k_mutex_lock(&temperature_mutex, K_FOREVER);
	set_sensor_value(tem_large, tem);
	k_mutex_unlock(&temperature_mutex);
}

static void generate_temperature(void)
{
	while (1) {
		update_temperature(&_temperature);
		k_sleep(K_SECONDS(TEM_GEN_DELAY_S));
	}
}
K_THREAD_DEFINE(temperature_thread_id, 1024, generate_temperature, NULL, NULL,
		NULL, 5, 0, 0);

void get_temperature(struct sensor_value *tem)
{
	if (k_mutex_lock(&temperature_mutex, K_MSEC(5)) == 0) {
		tem->val1 = _temperature.val1;
		tem->val2 = _temperature.val2;
		k_mutex_unlock(&temperature_mutex);
	}
}
