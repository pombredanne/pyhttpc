#
# Copyright 2009 Paul J. Davis <paul.joseph.davis@gmail.com>
#
# This file is part of the python-spidermonkey package released
# under the MIT license.
#

"""\
An HTTP parser written in C. Based on the HTTP parser by Ryan Dahl of
Node.js fame.

http://github.com/davisp/pyhttpc/
http://github.com/ry/http-parser/
""",

import ez_setup
ez_setup.use_setuptools()
from setuptools import setup, Extension

setup(
    name = "pyhttpc",
    version = "0.0.1",
    license = "MIT",
    author = "Paul J. Davis",
    author_email = "paul.joseph.davis@gmail.com",
    description = "HTTP Parser",
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
            "./src/parser.c"
        ])
    ],

    test_suite = 'nose.collector',

)
