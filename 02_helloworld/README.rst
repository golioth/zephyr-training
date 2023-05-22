Section 2: Hello World
######################

The Hello World sample has been copied directly `from the Zephyr tree`_. It does
almost nothing, merely printing out the name of the board for which the app was
built to the serial terminal.

Using this sample you will learn:

* Kconfig symbols are accessible to your programs
* How to use ``menuconfig`` and C to enable logging

Hardware
********

The committed code should build for all Zephyr-supported boards.

Build Instructions
******************

Prerequisites
=============

Follow the README in the root of this repository to install a
virtual environment and use ``west init``/``west update`` to clone all dependencies.
Remember to restart your virtual environment when beginning a new terminal
session.

Build project
=============

From the root directory of this repository, build using the following command:

.. code-block:: bash

   $ # Building for nRF7002dk
   $ (.venv) west build -b nrf7002dk_nrf5340_cpuapp 02_helloworld

   $ # Building for nRF9160dk
   $ (.venv) west build -b nrf9160dk_nrf9160_ns 02_helloworld

Flash project
=============

Plug the nRF7002-DK into the USB port on the short edge of the board. (Do not
use the USB port labelled ``nRF USB``.) Run the following command:

.. code-block:: bash

   $ (.venv) west flash

Application Behavior
********************

When the app begins running it print "Hello World!" followed by the name of the
board for which it was built:

.. code-block::

   *** Booting Zephyr OS build fcaa60a99fa9 ***
   Hello World! nrf7002dk_nrf5340_cpuapp

.. _from the Zephyr tree: https://github.com/zephyrproject-rtos/zephyr/tree/main/samples/hello_world
