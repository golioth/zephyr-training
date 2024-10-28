Section 4: Blink with RTOS
##########################

The Blink with RTOS sample has been copied `from the Zephyr tree blinky
sample`_, with custom overlay mapping that you implemented in the previous
sample. Currently this will blink an LED at 1 Hz, but your mission (if you
choose to accept it) is to try out two different RTOS features to manage the
blinking.

Using this sample you will learn:

* How to use a Zephyr timer and interrupt
* How to use a Zephyr thread
* How to view thread stack usage with the Zephyr shell

Hardware
********

This code will build for all Zephyr-supported boards that have
an overlay file which defines the ``training_led`` alias. Overlay files for
``nRF7002DK`` and ``nRF9160dk`` have been provided.

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
   $ (.venv) west build -b nrf7002dk/nrf5340/cpuapp 03_LED

   $ # Building for nRF9160dk
   $ (.venv) west build -b nrf9160dk/nrf9160/ns 03_LED

Flash project
=============

Connect your computer to the board via USB. (On the nRF7002dk, do not use the
USB port labelled `nRF USB`.) Run the following command:

.. code-block:: bash

   $ (.venv) west flash

Application Behavior
********************

When the app begins running it blink the onboard LED at 1 Hz.

.. _from the Zephyr tree: https://github.com/zephyrproject-rtos/zephyr/tree/main/samples/basic/blinky
