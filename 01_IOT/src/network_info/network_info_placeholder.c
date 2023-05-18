/*
 * Copyright (c) 2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(golioth_iot, LOG_LEVEL_DBG);

#include <network_info.h>

int __attribute__((weak)) network_info_init(void) {
	return 0;
}

int __attribute__((weak)) network_info_add_to_map(QCBOREncodeContext *response_detail_map)
{
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "No Network Info",
				     "Network reporting not implemented "
				     "for this board");
	return 0;
}

int __attribute__((weak)) network_info_log(void)
{
	LOG_DBG("====== Network Info ======");

	LOG_DBG("State: %s", "No Network Info;");
	LOG_DBG("%s", "Network reporting not implemented for this board");
	LOG_DBG("==========================");
	return 0;
}
