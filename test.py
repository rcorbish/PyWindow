#!/usr/bin/env python

import numpy as np
import sys
import time
from pywindow import PyWindow

win = PyWindow( width=300, height=200 )

a = ( np.random.rand( 5, 5, 4 ) * 255 ).astype( np.uint8 )
b = np.ones( np.shape(a), dtype=np.uint8 )

for x in range( np.shape(a)[0] ) :
    for y in range( np.shape(a)[1] ) :
        a[x][y][3] = 1
        b[x][y][3] = 0
    
while win.show() :
    a = a + b
    win.set_image( a )
    
    time.sleep( 0.001 )
