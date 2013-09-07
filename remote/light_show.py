#!/usr/bin/env python

import socket
import struct
from time import sleep

class LightsConnection(object):
    def __init__(self, lights_host=(("localhost", 4908))):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect(lights_host)
        self.s.send("safety off")
        self.number_of_lights = struct.unpack("!i", self.s.recv(4, socket.MSG_WAITALL))[0]

    def write_all_lights(self, lights):
        if len(lights) != self.number_of_lights:
            raise ValueError, "Write all lights has to write exactly all the lights"

        self.s.send(struct.pack("!" + "i"*self.number_of_lights, *lights))

if __name__ == '__main__':
    l = LightsConnection()
    print l.number_of_lights
    print "All off..."
    l.write_all_lights((0,)*l.number_of_lights)
    sleep(1)
    print "All on..."
    l.write_all_lights((0x00ffffff,)*l.number_of_lights)
    sleep(1)
    print "Done"
