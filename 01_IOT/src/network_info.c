/*
 * Copyright (c) 2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(net_info, LOG_LEVEL_DBG);

#include "network_info.h"

#ifdef CONFIG_MODEM_INFO
#include <modem/modem_info.h>
#endif

#ifdef CONFIG_BOARD_NRF7002DK_NRF5340_CPUAPP
#include "wifi_util.h"
#endif

int add_network_info_to_map(QCBOREncodeContext *response_detail_map)
{
#if defined CONFIG_BOARD_NRF7002DK_NRF5340_CPUAPP

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

#elif defined CONFIG_MODEM_INFO

	char sbuf[128];
	modem_info_string_get(MODEM_INFO_RSRP, sbuf, sizeof(sbuf));
	LOG_INF("Signal strength: %s", sbuf);
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "Signal strength",
				     sbuf);
	modem_info_string_get(MODEM_INFO_CUR_BAND, sbuf, sizeof(sbuf));
	LOG_INF("Current LTE band: %s", sbuf);
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "Current LTE band",
				     sbuf);
	modem_info_string_get(MODEM_INFO_SUP_BAND, sbuf, sizeof(sbuf));
	LOG_INF("Supported LTE bands: %s", sbuf);
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "Supported LTE bands",
				     sbuf);
	modem_info_string_get(MODEM_INFO_AREA_CODE, sbuf, sizeof(sbuf));
	LOG_INF("Tracking area code: %s", sbuf);
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "Tracking area code",
				     sbuf);
	modem_info_string_get(MODEM_INFO_UE_MODE, sbuf, sizeof(sbuf));
	LOG_INF("Current mode: %s", sbuf);
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "Current mode",
				     sbuf);
	modem_info_string_get(MODEM_INFO_OPERATOR, sbuf, sizeof(sbuf));
	LOG_INF("Current operator name: %s", sbuf);
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "Current operator name",
				     sbuf);
	modem_info_string_get(MODEM_INFO_CELLID, sbuf, sizeof(sbuf));
	LOG_INF("Cell ID of the device: %s", sbuf);
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "Cell ID of the device",
				     sbuf);
	modem_info_string_get(MODEM_INFO_IP_ADDRESS, sbuf, sizeof(sbuf));
	LOG_INF("IP address of the device: %s", sbuf);
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "IP address of the device",
				     sbuf);
	modem_info_string_get(MODEM_INFO_FW_VERSION, sbuf, sizeof(sbuf));
	LOG_INF("Modem firmware version: %s", sbuf);
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "Modem firmware version",
				     sbuf);
	modem_info_string_get(MODEM_INFO_LTE_MODE, sbuf, sizeof(sbuf));
	LOG_INF("LTE-M support mode: %s", sbuf);
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "LTE-M support mode",
				     sbuf);
	modem_info_string_get(MODEM_INFO_NBIOT_MODE, sbuf, sizeof(sbuf));
	LOG_INF("NB-IoT support mode: %s", sbuf);
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "NB-IoT support mode",
				     sbuf);
	modem_info_string_get(MODEM_INFO_GPS_MODE, sbuf, sizeof(sbuf));
	LOG_INF("GPS support mode: %s", sbuf);
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "GPS support mode",
				     sbuf);
	modem_info_string_get(MODEM_INFO_DATE_TIME, sbuf, sizeof(sbuf));
	LOG_INF("Mobile network time and date: %s", sbuf);
	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "Mobile network time and date",
				     sbuf);

#else

	QCBOREncode_AddSZStringToMap(response_detail_map,
				     "No Network Info",
				     "Network reporting not implemented "
				     "for this board");

#endif
	return 0;
}
