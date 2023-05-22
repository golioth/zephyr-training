Section 5: Add Golioth to Your Project
######################################

This sample demonstrates how to add Golioth to an existing project. It assumes
that you already have an internet connection set up, and thus this sample
includes code necessary for the nRF7002dk or nRF9160dk to establish a network
connection during powerup. It includes the timer and thread based LED blinking
code that you implemented in the previous sample.

Using this sample you will learn:

* How to build the Golioth Zephyr SDK into your project
* How to add the Golioth Client to your C files
* How to make LightDB State and LightDB Stream API calls so send data to Golioth

Hardware
********

This code will build and run on the ``nRF7002DK`` and ``nRF9160dk`` boards.

Build Instructions
******************

Prerequisites
=============

Follow the README in the root of this repository to install a virtual
environment and use ``west init``/``west update`` to clone all dependencies.
Remember to restart your virtual environment when beginning a new terminal
session.


Build project
=============

From the root directory of this repository, build using the following command:

.. code-block:: bash

   $ # Building for nRF7002dk
   $ (.venv) west build -b nrf7002dk_nrf5340_cpuapp 03_LED

   $ # Building for nRF9160dk
   $ (.venv) west build -b nrf9160dk_nrf9160_ns 03_LED

Flash project
=============

Plug the nRF7002-DK into the USB port on the short edge of the board. (Do not
use the USB port labelled ``nRF USB``.) Run the following command:

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

When the app begins running, LED1 and LED2 will begin blinking at different
rates. A network connection will be automatically established.

