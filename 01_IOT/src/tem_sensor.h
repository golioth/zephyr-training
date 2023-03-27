/*
 * Copyright (c) 2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __TEM_SENSOR_H__
#define __TEM_SENSOR_H__

#include <zephyr/drivers/sensor.h>

/**
 * @brief Get simulated temperature sensor data
 *
 * Temperature is generated with two decimal places of precision to simulate the
 * type of readings that would be received from a BME280 sensor.
 *
 * @param tem sensor_value point to store the new temperature values. val1 is
 * the integral part, val2 is the decimal part and returns six digits of
 * precision to adhere to the Zephyr sensor API.
 */
void get_temperature(struct sensor_value *tem);

#endif
