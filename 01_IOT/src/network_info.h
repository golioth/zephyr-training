/*
 * Copyright (c) 2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __NETWORK_INFO_H__
#define __NETWORK_INFO_H__

#include <qcbor/qcbor.h>

int add_network_info_to_map(QCBOREncodeContext *response_detail_map);

#endif
