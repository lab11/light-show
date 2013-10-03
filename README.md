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
run the lights with different applications.

    sudo apt-get install libjson0-dev


Remote Access
-------------

The app 'remote' in `c/apps/remote.c` provides network-controllable access for
the lights. Control of the strand is retained by the remote application for up
to 60 seconds, you may give up your quanta earlier by closing the connection. To
ensure responsiveness, a remote application must send a message every 3 seconds
or it will be dropped.

`remote/light_show.py` provides a basic python class and example for controlling
the lights over the remote interface.
