/*
 * Copyright (c) 2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __LED_BLINK_H__
#define __LED_BLINK_H__

#include <stdint.h>

void led_set_selected(uint8_t sel);
void led_wake_thread(void);
void led_set_delay(uint32_t delay);

#endif
