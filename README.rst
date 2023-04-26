Golioth Zephyr Training: nRF7002-DK
###################################

Overview
********

This repository contains the code for Golioth's self-guided Zephyr training
using the nRF7002-DK. Please visit `Golioth's Developer Training Docs
<https://training.golioth.io>`_ to learn Zephyr with us.

Local set up
************

Do not clone this repo using git. Zephyr's ``west`` meta tool should be used to
set up your local workspace.

Install the Python virtual environment (recommended)
====================================================

.. code-block:: console

   cd ~
   mkdir zephyr-training-nrf7002dk
   python -m venv zephyr-training-nrf7002dk/.venv
   source zephyr-training-nrf7002dk/.venv/bin/activate
   pip install wheel west

Use ``west`` to initialize and install
======================================

.. code-block:: console

   cd ~/zephyr-training-nrf7002dk
   west init -m git@github.com:golioth/zephyr-training-nrf7002dk.git .
   west update
   west zephyr-export
   pip install -r deps/zephyr/scripts/requirements.txt

Building the Training Applications
**********************************

Each numbered directory contains a different applications used in the training.

Storing Device Credentials
==========================

After flashing firmware to the device (see below) you will need to store your
WiFi and Golioth credentials in flash memory. Perform this task using a serial
terminal and the following commands:

.. code-block::

   uart:~$ settings set wifi/ssid <my-wifi-ap-ssid>
   uart:~$ settings set wifi/psk <my-wifi-ap-password>
   uart:~$ settings set golioth/psk-id <my-psk-id@my-project>
   uart:~$ settings set golioth/psk <my-psk>
   uart:~$ kernel reboot cold

These credentials will be stored in the settings partition and will survive new
application firmware flashed directly, or via OTA update.

Note: The WiFi security type must be specified as a Kconfig command. Looks for a
section on this in the prj.conf file. The default is WPA2.

01-IOT
======

.. code-block:: console

   # Enable the Python virtual environment
   $ cd ~/zephyr-training-nrf7002dk
   $ source .venv/bin/activate

   # Build the application
   $ (.venv) cd app
   $ (.venv) west build -b nrf7002dk_nrf5340_cpuapp 01_IOT

   # Flash the binary to your device
   $ (.venv) west flash

Using Kasm
**********

When using Kasm, the compiled binary must be downloaded and flashed to your
device locally using the `nRF Connect for Desktop
<https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-desktop/>`
programmer app.

Use the following command to copy a compiled hex file to the Kasm download folder.
(A timestamp and the name of the app will be added to the filename.)

.. code-block:: console

   $ (.venv) west kasm download
