/*
 * Copyright (c) 2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(net_info, LOG_LEVEL_DBG);

#include <network_info.h>
#include "../wifi_util.h"

int network_info_init(void) {
	return 0;
}

int network_info_add_to_map(QCBOREncodeContext *response_detail_map)
{
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
	return 0;
}

int network_info_log(void)
{
	struct wifi_iface_status w_status = { 0 };

	cmd_wifi_status(&w_status);

	LOG_DBG("====== WiFi Network Info ======");
	LOG_DBG("State: %s", wifi_state_txt(w_status.state));

	if (w_status.state >= WIFI_STATE_ASSOCIATED) {
		uint8_t mac_string_buf[sizeof("xx:xx:xx:xx:xx:xx")];
		LOG_DBG("Interface Mode: %s", wifi_mode_txt(w_status.iface_mode));
		LOG_DBG("Link Mode: %s", wifi_link_mode_txt(w_status.link_mode));
		LOG_DBG("SSID: %s", w_status.ssid);
		LOG_DBG("BSSID: %s",
			net_sprint_ll_addr_buf(w_status.bssid,
					       WIFI_MAC_ADDR_LEN,
					       mac_string_buf,
					       sizeof(mac_string_buf))
					       );
		LOG_DBG("Band: %s", wifi_band_txt(w_status.band));
		LOG_DBG("Channel: %d", w_status.channel);
		LOG_DBG("Securit: %sy", wifi_security_txt(w_status.security));
		LOG_DBG("MFP: %s", wifi_mfp_txt(w_status.mfp));
		LOG_DBG("RSSI: %d", w_status.rssi);
	}
	LOG_DBG("===============================");
	return 0;
}
