# Network Info

This folder holds device-specific files for returning network info. Only one of
these files should be built into a project. This is selected using the
CMakeLists.txt and Kconfg files.

## Adding a network_info file for a new board

1. Create the new C file
  1. add `#include <network_info.h>`
  2. implement required functions
    * `int network_info_init(void)`
    * `int network_info_add_to_map(QCBOREncodeContext *response_detail_map)`
    * `int network_info_log(void)`
2. Add C file to CMakeLists.txt gated by the Kconfig symbol for that file:

    ```
    zephyr_library_sources_ifdef(CONFIG_BOARD_NRF7002DK_NRF5340_CPUAPP network_info_nrf7002dk.c)
    zephyr_library_sources_ifdef(CONFIG_MODEM_INFO network_info_modem_info.c)
    ```
