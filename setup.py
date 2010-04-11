# Copyright 2009 Paul J. Davis <paul.joseph.davis@gmail.com>
#
# This file is part of the PyHTTPC pacakge released under the MIT license.

"""\
An HTTP parser written with Ragel.
"""

import os
import subprocess as sp
import sys
import textwrap
DD = textwrap.dedent

try:
    import setuptools
except ImportError:
    from ez_setup import use_setuptools
    use_setuptools()

from setuptools import setup, find_packages, Extension, Feature
from distutils.command.build_ext import build_ext
from distutils.errors import CCompilerError, DistutilsExecError, \
        DistutilsPlatformError


# I haven't the sligthest, but this appears to fix
# all those EINTR errors. Pulled and adapted for OS X
# from twisted bug #733
if os.uname()[0] == "Darwin":
    import ctypes
    import signal
    libc = ctypes.CDLL("libc.dylib")
    libc.siginterrupt(signal.SIGCHLD, 0)

# Update our ragel state machines if need be.
# Include a dependency on http.rl for everythin
# as its included.
def ragel(rlfname):
    cfname = os.path.splitext(rlfname)[0] + ".c"
    if os.path.isfile(cfname):
        rlmt = os.stat("./c_src/http.rl").st_mtime
        rlmt = max(rlmt, os.stat(rlfname).st_mtime)
        cmt = os.stat(cfname).st_mtime
        if rlmt < cmt:
            return
    sp.check_call(["ragel", "-C", "-o", cfname, rlfname])
ragel("./c_src/request.rl")

native = Feature(
    "Optionally build the native C code.",
    standard=True,
    ext_modules = [
        Extension("pyhttpc.native", [
            "./c_src/native.c",
            "./c_src/request.c"
        ]),
    ],
)

class BuildFailed(Exception):
    pass

class OptionalBuilder(build_ext):
    # This class allows C extension building to fail.

    ext_errors = [CCompilerError, DistutilsExecError, DistutilsPlatformError]
    if sys.platform == 'win32' and sys.version_info > (2, 6):
        ext_errors.append(IOError)
    ext_errors = tuple(ext_errors)

    def run(self):
        try:
            build_ext.run(self)
        except DistutilsPlatformError, x:
            raise BuildFailed()

    def build_extension(self, ext):
        try:
            build_ext.build_extension(self, ext)
        except self.ext_errors, x:
            raise BuildFailed()

def run_setup(with_binary):
    if with_binary:
        features = {'native-parser': native}
    else:
        features = {}

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
    
        features = features,
        cmdclass = {'build_ext': OptionalBuilder},
    
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

        test_suite = 'nose.collector',
    )

try:
    run_setup(True)
except BuildFailed:
    print "*" * 75
    print "WARNING: The C extension could not be compiled."
    print "Failure information, if any, is above."
    print "I'm retrying the build without the C extension now."
    print '*' * 75

    run_setup(False)

    print '*' * 75
    print "Plain-Python installation succeeded."
    print '*' * 75
