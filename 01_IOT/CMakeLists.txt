# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(golioth_iot)

target_include_directories(app PUBLIC ${ZEPHYR_BASE}/subsys/net/ip)

zephyr_include_directories(src/include)
zephyr_library_sources_ifdef(CONFIG_BOARD_NRF7002DK_NRF5340_CPUAPP src/wifi_util.c)

target_sources(app PRIVATE
	src/main.c
	src/led_blink.c
	src/tem_sensor.c
)
