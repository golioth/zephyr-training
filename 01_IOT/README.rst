Section 1: IoT
##############

The IoT sample demonstrates an IoT device that connects to WiFi and Golioth,
then sends data, logs, and reacts to remote-procedure calls. Using this sample
code you will learn about:

* Building and flashing applications in Zephyr
* Adding persistent credentials using the serial shell
* Get to know the Golioth web console:
   * Time-series and stateful data reported to the cloud
   * Adjusting device settings using the cloud-side Settings service
   * Receiving remote logging information from the device
   * Using remote procedure call (RPC) to return current WiFi connection info

Hardware: Nordic nRF7002-DK
***************************

This demonstrates how to interact with the `Golioth Cloud`_ using `Zephyr`_ on the
`Nordic nRF7002 Development Kit`_.


Build Instructions
******************

Prerequisites
=============

Follow the README in the root of this repository to install a
virtual environment and use `west init`/`west update` to clone all dependencies.
Remember to restart your virtual environment when beginning a new terminal
session.


Build project
=============

From the root directory of this repository, build using the following command:

.. code-block:: bash

   $ (.venv) west build -b nrf7002dk_nrf5340_cpuapp 01_IOT

Flash project
=============

Plug the nRF7002-DK into the USB port on the short edge of the board. (Do not
use the USB port labelled `nRF USB`.) Run the following command:

.. code-block:: bash

   $ (.venv) west flash

Add credentials
===============

Two serial ports will enumerate on your system. Use a serial terminal program to
connect to the higher numbered serial port using 115200 8N1 as the settings. Use
the following syntax to store your WiFi and Golioth credentials on the device:

.. code-block:: bash

   uart:~$ settings set wifi/ssid <my-wifi-ap-ssid>
   uart:~$ settings set wifi/psk <my-wifi-ap-password>
   uart:~$ settings set golioth/psk-id <my-psk-id@my-project>
   uart:~$ settings set golioth/psk <my-psk>
   uart:~$ kernel reboot cold

Application Behavior
********************

When the app begins running it will connect to WiFi, then connect to Golioth and
begin streaming simulated temperature sensor data:

.. code-block::

   *** Booting Zephyr OS build fcaa60a99fa9 ***
   [00:00:00.466,949] <inf> golioth_system: Initializing
   [00:00:00.470,764] <inf> fs_nvs: 6 Sectors of 4096 bytes
   [00:00:00.470,764] <inf> fs_nvs: alloc wra: 0, f78
   [00:00:00.470,794] <inf> fs_nvs: data wra: 0, cc
   [00:00:00.471,282] <inf> net_config: Initializing network
   [00:00:00.471,282] <inf> net_config: Waiting interface 1 (0x200018d8) to be up...
   [00:00:00.472,106] <inf> net_config: IPv4 address: 192.168.1.99
   [00:00:00.472,167] <inf> net_config: Running dhcpv4 client...
   [00:00:00.474,731] <dbg> golioth_iot: main: Start Golioth IoT
   [00:00:00.474,761] <inf> golioth_iot: Starting nrf7002dk_nrf5340_cpuapp with CPU frequency: 64 MHz
   [00:00:00.474,975] <inf> wpa_supp: z_wpas_start: 385 Starting wpa_supplicant thread with debug level: 3

   [00:00:00.475,189] <inf> wpa_supp: Successfully initialized wpa_supplicant
   [00:00:01.474,884] <inf> golioth_iot: Static IP address (overridable): 192.168.1.99/255.255.255.0 -> 192.168.1.1
   [00:00:03.078,918] <inf> golioth_iot: Connection requested
   [00:00:06.664,428] <inf> wpa_supp: wlan0: SME: Trying to authenticate with c6:ff:d4:a8:fa:10 (SSID='YourWiFiSSID' freq=2437 MHz)
   [00:00:06.667,877] <inf> wifi_nrf: wifi_nrf_wpa_supp_authenticate:Authentication request sent successfully

   [00:00:06.923,370] <inf> wpa_supp: wlan0: Trying to associate with c6:ff:d4:a8:fa:10 (SSID='YourWiFiSSID' freq=2437 MHz)
   [00:00:06.932,128] <inf> wifi_nrf: wifi_nrf_wpa_supp_associate: Association request sent successfully

   [00:00:06.948,394] <inf> wpa_supp: wlan0: Associated with c6:ff:d4:a8:fa:10
   [00:00:06.948,547] <inf> wpa_supp: wlan0: CTRL-EVENT-SUBNET-STATUS-UPDATE status=0
   [00:00:06.972,045] <inf> wpa_supp: wlan0: WPA: Key negotiation completed with c6:ff:d4:a8:fa:10 [PTK=CCMP GTK=CCMP]
   [00:00:06.972,290] <inf> wpa_supp: wlan0: CTRL-EVENT-CONNECTED - Connection to c6:ff:d4:a8:fa:10 completed [id=0 id_str=]
   [00:00:06.979,614] <inf> golioth_iot: Connected
   [00:00:06.981,872] <inf> golioth_system: Starting connect
   [00:00:06.984,283] <err> golioth: Fail to get address (coap.golioth.io 5684) -11
   [00:00:06.984,313] <err> golioth_system: Failed to connect: -11
   [00:00:06.984,313] <wrn> golioth_system: Failed to connect: -11
   [00:00:07.001,159] <inf> net_dhcpv4: Received: 192.168.1.127
   [00:00:07.001,281] <inf> net_config: IPv4 address: 192.168.1.127
   [00:00:07.001,281] <inf> net_config: Lease time: 43200 seconds
   [00:00:07.001,312] <inf> net_config: Subnet: 255.255.255.0
   [00:00:07.001,373] <inf> net_config: Router: 192.168.1.1
   [00:00:07.002,197] <inf> golioth_iot: DHCP IP address: 192.168.1.127
   [00:00:11.984,436] <inf> golioth_system: Starting connect
   [00:00:12.360,626] <inf> golioth_iot: Sending hello! 0
   [00:00:12.362,884] <inf> golioth_system: Client connected!
   [00:00:12.362,213] <inf> golioth_iot: Streaming Temperature to Golioth: 28.060000
   [00:00:17.362,457] <inf> golioth_iot: Sending hello! 1
   [00:00:17.363,677] <inf> golioth_iot: Streaming Temperature to Golioth: 27.440000

Button and LEDs
===============

One LED will always be blinking. Pressing the user buttons will change which LED
is currently blinking

Time-Series and Stateful Data
=============================

Simulated temperature readings will be sent periodically to Golioth. Each of
these readings is recorded with a timestamp when it was received.

When the user buttons are pressed to change which LED is blinking, the name of
the currently blinking LED (`LED` or `LED2`) will be recorded on the Golioth
LightDB State service.

Sensor Reading and LED Blinking Frequency
=========================================

The Golioth Settings Service determines the frequency at which simulated
temperature readings are sent and the frequency at which the LED blinks. The
following keys are used for these settings:

* Sensor frequency (seconds): `LOOP_DELAY_S`
* Blink frequency (milliseconds): `BLINK_DELAY_MS`

Reporting Network Connection Information
=====================================

A remote procedure call can be used to return information about the network to
which the device is currently connected:

* `get_network_info`

.. image:: img/rpc_network_info.jpg

.. _Golioth Cloud: https://golioth.io/
.. _Zephyr: https://www.zephyrproject.org/
.. _Nordic nRF7002 Development Kit: https://www.nordicsemi.com/Products/Development-hardware/nRF7002-DK

