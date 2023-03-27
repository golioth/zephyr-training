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

Prerequisite: Credentials
=========================

One credentials file may be used for all sample applications that require WiFi
and Golioth credentials. Make a copy of ``credentials.conf_example``, rename it
to ``credentials.conf`` and populate it with the following information:

* The SSID and PSK (password) for your WiFi access point
* Select the type of PSK encryption used by your access point by uncommenting
  one of the lines in that file (WPA2 is selected by default)
* Golioth device credentials

  * Visit the `Golioth Console <https://console.golioth.io>`_ to create/view
    your device credentials. (With Golioth's Dev Tier your first 50 devices are
    free.)

The credentials file is ignored and will not be included in commits.

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
