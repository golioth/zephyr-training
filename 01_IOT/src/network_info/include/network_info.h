/*
 * Copyright (c) 2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __NETWORK_INFO_H__
#define __NETWORK_INFO_H__

#include <qcbor/qcbor.h>

/**
 * @brief Initialize the Network Info file for this board
 *
 * @retval 0 On success
 * @retval <0 On failure
 */
int network_info_init(void);

/**
 * @brief Populate a QCBOR map with Network Information
 *
 * Query the network information for this board and write the results to a QCBOR
 * map.
 *
 * @param response_detail_map Pointer to a QCBOR map where network information
 * should be written
 *
 * @retval 0 On success
 * @retval <0 On failure
 */
int network_info_add_to_map(QCBOREncodeContext *response_detail_map);

/**
 * @brief Write network information as a LOG message
 *
 * @retval 0 On success
 * @retval <0 On failure
 */
int network_info_log(void);

#endif
