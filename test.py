#!/usr/bin/env python

import numpy as np
import sys
import time
from pywindow import PyWindow
import math

win = PyWindow( width=300, height=200 )

a = ( np.random.rand( 100, 100, 4 ) * 255 ).astype( np.uint8 )
for x in range( np.shape(a)[0] ) :
    for y in range( np.shape(a)[1] ) :
        a[x][y][3] = 1
    
o1 = 0 
o2 = 0
while win.show() :
    for x in range( np.shape(a)[0] ) :
        for y in range( np.shape(a)[1] ) :
            a[x][y][0] = x + o1 
            a[x][y][1] = y + o2
            # a[x][y][1] = math.cos( y + o2 ) * 255
            # a[x][y][2] = math.cos( x + o2 ) + math.sin( y + o1 ) * 200
    o1 = o1 + 3
    o2 = o2 + 1

    win.set_image( a )
    
    # time.sleep( 0.01 )
