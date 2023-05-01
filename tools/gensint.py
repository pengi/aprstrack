#!/usr/bin/python

from math import sin,pi

for t in range(96):
    tf = t/96.0*pi*2
    vf = sin(tf)
    v = 25 + 20*vf
    print " %3d," % (v,),
    if t % 8 == 7:
        print ""
