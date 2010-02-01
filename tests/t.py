# Copyright 2009 Paul J. Davis <paul.joseph.davis@gmail.com>
#
# This file is part of the pywebmachine package released
# under the MIT license.
import inspect
import os
import re
import sys
import tempfile
import unittest

import pyhttpc

dirname = os.path.dirname(__file__)

class FakeSocket(object):
    def __init__(self, fname):
        self.tmp = tempfile.TemporaryFile()
        
        lines = []
        for line in open(fname, "ru").readlines():
            line = line.rstrip("\n").replace("\\r\\n", "\r\n")
            lines.append(line)
        #sys.stderr.write("%r" % ''.join(lines))
        self.tmp.write(''.join(lines))
        self.tmp.flush()
        self.tmp.seek(0)

    def fileno(self):
        return self.tmp.fileno()
        
    def len(self):
        return self.tmp.len
        
    def recv(self, length=None):
        return self.tmp.read()
        
    def send(self, data):
        self.tmp.write(data)
        self.tmp.flush()
        
    def seek(self, offset, whence=0):
        self.tmp.seek(offset, whence)

class request(object):
    def __init__(self, name, eol="\r\n"):
        self.fname = os.path.join(dirname, "requests", name)

    def __call__(self, func):
        def run():
            sock = FakeSocket(self.fname)
            parser = pyhttpc.parse_requests(sock)
            func(parser)
        run.func_name = func.func_name
        return run
    
def eq(a, b):
    assert a == b, "%r != %r" % (a, b)

def ne(a, b):
    assert a != b, "%r == %r" % (a, b)

def lt(a, b):
    assert a < b, "%r >= %r" % (a, b)

def gt(a, b):
    assert a > b, "%r <= %r" % (a, b)

def isin(a, b):
    assert a in b, "%r is not in %r" % (a, b)

def isnotin(a, b):
    assert a not in b, "%r is in %r" % (a, b)

def has(a, b):
    assert hasattr(a, b), "%r has no attribute %r" % (a, b)

def hasnot(a, b):
    assert not hasattr(a, b), "%r has an attribute %r" % (a, b)

def raises(exctype, func, *args, **kwargs):
    try:
        func(*args, **kwargs)
    except exctype, inst:
        pass
    else:
        func_name = getattr(func, "func_name", "<builtin_function>")
        raise AssertionError("Function %s did not raise %s" % (
            func_name, exctype.__name__))

