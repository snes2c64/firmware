.. SNES2C64 documentation master file, created by
   sphinx-quickstart on Thu Jan 19 12:17:03 2023.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to SNES2C64's documentation!
====================================

.. toctree::
   :maxdepth: 2
   :caption: Contents:



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

Overview
========

This Project is an highly configurable adapter to use a SNES controller as a Commodore 64 Joystick.

It contains of multiple Parts:

 - The Hardware, which is basically
  - a Arduino Nano
  - 2 LED's with resistors
  - a 9 pin D-Sub connector to connect to the C64
  - a SNES connector
  - and last but not least a PCB kindly layouted by @OliverW
 - Firmware for the Arduino Nano
 - a cli application to configure the adapter
 - a gui application to configure the adapter
