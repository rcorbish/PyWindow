#!/usr/bin/env python
import threading
import numpy as np
import sys
import time
from pywindow import PyWindow

win = 0 

def glutMainLoop( win2 ) :
    win = PyWindow( width=200, height=200 )
    a = np.random.rand( 10,10,4 )
    win.set_image( a )
    print( "Starting thread" )
    win.show() 
    print( "Ending thread" )

t = threading.Thread( target=glutMainLoop, args=(win,) )
t.start() 

# glutMainLoop()

# while True :
#     a = np.random.rand( 10,10,4 )
#     win.set_image( a )
#     time.sleep( 0.2 )
