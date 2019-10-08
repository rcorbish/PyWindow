#!/usr/bin/env python
import numpy
from distutils.core import setup, Extension

pywindow_module = Extension(
    'pywindow',
    sources=['pywindow.cpp'],
    extra_compile_args = ['--std=c++17', '-g3', '-O0', '-Wno-write-strings' ],
    libraries = ['stdc++', 'glut', 'GLU' ],
    language='C++17', )

setup(
    name='pywindow', 
    version='0.1.0',
    description='Allow direct updates from arrray to screen',
    ext_modules=[ pywindow_module ],
    include_dirs=[ numpy.get_include() ],
    )
