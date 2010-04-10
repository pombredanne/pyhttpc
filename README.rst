PyHttpC
=======

A small HTTP parser in C for Python. Based on the HTTP parser in
Zed Shaw's Mongrel.

Status
------

This is some very very preliminary work to at least demonstrate the API
design I have in mind. Its easily broken and I have yet to figure out how
exactly I want to do body parsing. To be WSGI compatible I basically need
to stop parsing after the headers and return something that can read
from the socket or read the body entirely. Perhaps make that configurable?

Building
--------
::

    $ git clone git://github.com/davisp/pyhttpc.git
    $ cd pyhttpc
    $ python setup.py build
    $ python setup.py test

