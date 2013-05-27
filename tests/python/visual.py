#!/usr/bin/env python3
# OneJoker library <http://lcrocker.github.io/OneJoker>
#
# "Hello World" app included here to keep docs in sync with code.

import os, sys, time
sys.path.insert(1, os.path.dirname(os.path.dirname(__file__)))

from ctypes import *
from ctypes.util import find_library

ojlib = CDLL(find_library("ojcard"))
ojlib.ojc_binomial.restype = c_longlong
ojlib.ojr_next_double.restype = c_double

# import onejoker as oj

from tkinter import *
from tkinter.ttk import *

def fill_image():
    for line in range(512):
        cols = []
        for col in range(512):
            if line < 128:
                rand = ojlib.ojr_next32()
                r = (rand >> 16) & 255
                g = (rand >> 8) & 255
                b = rand & 255
            elif line < 256:
                p = im.get(col, line)
                r, g, b = (int(x) for x in p.split())
                rand = ojlib.ojr_next32()
                r ^= (rand >> 16) & 255
                g ^= (rand >> 8) & 255
                b ^= rand & 255
            elif line < 384:
                r = ojlib.ojr_rand(256)
                g = ojlib.ojr_rand(256)
                b = ojlib.ojr_rand(256)
            else:
                r = int(256.0 * ojlib.ojr_next_double())
                g = int(256.0 * ojlib.ojr_next_double())
                b = int(256.0 * ojlib.ojr_next_double())

            cols.append("#{0:02x}{1:02x}{2:02x}".format(r, g, b))

        s = "{" + " ".join(cols) + "}"
        im.put(s, to = (0, line))
        root.after(10, tickgen.__next__)
        yield
    root.after(2000, root.quit)
    yield # tkinter callbacks don't like getting StopIteration

tickgen = fill_image()

root = Tk()
im = PhotoImage(file = "jeenie11.ppm")
c = Canvas(root, height = 512, width = 512)
c.create_image((0, 0), anchor = NW, image = im)
c.pack()
fill_image()

tickgen.__next__()
root.mainloop()
