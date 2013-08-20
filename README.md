Light Show
==========

This repository contains material to use one of the pseudo-addressable LED light
strips (based on the WS2801) with a Raspberry Pi.

Hardware
--------

In the hardware folder is a board design for a simple Raspberry Pi shield
that makes it easy to connect the lights to the RPi and an external power
supply.

Kernel Module
-------------

The kernel module is a simple driver for the lights that exports a character
device interface that an application can write to.

    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-

C Code
------

The C code is a little bit of a hodgepodge of different applications that
run the lights with different applications. tracer.c is designed to be used
with the GATD system.
