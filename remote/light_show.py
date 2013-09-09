#!/usr/bin/env python

import sys
import socket
import struct
from time import sleep
import threading

DEFAULT_HOST = 'localhost'
DEFAULT_PORT = 4908

class LightsConnection(object):
    BLUE  = 0xff000000
    GREEN = 0x00ff0000
    RED   = 0x0000ff00

    def __init__(self, lights_host=(("localhost", 4908))):
        self.s_lock = threading.Lock()
        self.s_lock.acquire()

        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect(lights_host)
        self.s.send("safety off")
        self.number_of_lights = struct.unpack("!i", self.s.recv(4, socket.MSG_WAITALL))[0]

        self.s_lock.release()

    def disconnect(self):
        self.s_lock.acquire()
        self.s.shutdown(socket.SHUT_RDWR)
        self.s.close()
        self.s_lock.release()

    def send_keepalive(self):
        self.s_lock.acquire()
        self.s.send('k')
        self.s_lock.release()

    def write_all_lights(self, lights):
        if len(lights) != self.number_of_lights:
            raise ValueError, "Write all lights has to write exactly all the lights"

        self.s_lock.acquire()
        #flags = socket.MSG_MORE -- not in python? w/e
        flags = 0
        self.s.send('a', flags)
        self.s.send(struct.pack("!" + "I"*self.number_of_lights, *lights))
        self.s_lock.release()

    def set_all_lights_to(self, val):
        self.write_all_lights((val,)*self.number_of_lights)

    def build_color_RGB(self, red, green, blue):
        return ((blue & 0xff) << 24) | ((green & 0xff) << 16) | ((red & 0xff) << 8)


class LightsConnectionWithKeepalives(LightsConnection):
    """Helper class for interacting with the lights in an interpreter session.

    The light strip times out a remote session after 5 seconds of inactivity.
    This can be hard to keep up with in a interactive session. This class spins
    off a background thread to continually send keep-alive messages.
    """

    keepalive_delay = 1

    def __init__(self, *args, **kwds):
        super(LightsConnectionWithKeepalives, self).__init__(*args, **kwds)

        self.stop_event = threading.Event()
        t = threading.Thread(target=self.keepalive_sender)
        t.daemon = True
        t.start()

    def disconnect(self):
        self.stop_event.set()
        return super(LightsConnectionWithKeepalives, self).disconnect()

    def keepalive_sender(self):
        while True:
            self.send_keepalive()
            if (self.stop_event.wait(self.keepalive_delay)):
                return

if __name__ == '__main__':
    try:
        DEFAULT_HOST = sys.argv[1]
        DEFAULT_PORT = sys.argv[2]
    except IndexError:
        pass
    l = LightsConnectionWithKeepalives((DEFAULT_HOST, DEFAULT_PORT))
    print l.number_of_lights
    print "All off..."
    l.write_all_lights((0,)*l.number_of_lights)
    sleep(3)
    print "All on..."
    l.write_all_lights((0xffffff00,)*l.number_of_lights)
    sleep(3)
    l.disconnect()
    print "Done"
