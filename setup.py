# Copyright 2009 Paul J. Davis <paul.joseph.davis@gmail.com>
#
# This file is part of the PyHTTPC pacakge released under the MIT license.

"""\
An HTTP parser written with Ragel.
"""
# I haven't the sligthest, but this appears to fix
# all those EINTR errors. Pulled and adapted for OS X
# from twisted bug #733
# 
# Definitely forgot to comment this out before distribution.
#
import ctypes
import signal
libc = ctypes.CDLL("libc.dylib")
libc.siginterrupt(signal.SIGCHLD, 0)


import os
import subprocess as sp

import ez_setup
ez_setup.use_setuptools()
from setuptools import setup, Extension

def ragel(rlfname, cfname):
    if os.path.isfile(cfname):
        rlmt = os.stat(rlfname).st_mtime
        cmt = os.stat(cfname).st_mtime
        if rlmt < cmt:
            return
    sp.check_call(["ragel", "-C", "-o", cfname, rlfname])
ragel("./src/request.rl", "./src/request.c")

setup(
    name = "pyhttpc",
    version = "0.0.1",
    license = "MIT",
    author = "Paul J. Davis",
    author_email = "paul.joseph.davis@gmail.com",
    description = "An HTTP Parser",
    long_description = __doc__,
    url = "http://github.com/davisp/pyhttpc",
    download_url = "http://github.com/davisp/pyhttpc.git",
    zip_safe = False,
    
    classifiers = [
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Natural Language :: English',
        'Operating System :: OS Independent',
        'Programming Language :: C',
        'Programming Language :: Python',
        'Topic :: Internet :: WWW/HTTP',
    ],
    
    setup_requires = [
        'setuptools>=0.6c8',
        'nose>=0.10.0',
    ],

    ext_modules = [
        Extension("pyhttpc", sources=[
            "./src/pyhttpc.c",
            "./src/request.c"
        ])
    ],

    test_suite = 'nose.collector',

)
