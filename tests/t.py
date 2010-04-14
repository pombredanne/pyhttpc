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

from pyhttpc import RequestParser

dirname = os.path.dirname(__file__)

class request(object):
    def __init__(self, name, eol="\r\n"):
        self.fname = os.path.join(dirname, "data", "requests", name)

    def __call__(self, func):
        def run():
            with open(self.fname) as handle:
                data = handle.read()
            data = data.replace("\n", "").replace("\\r\\n", "\r\n")

            def all_at_once():
                yield data
            func(RequestParser(all_at_once()))

            def line_at_a_time():
                lines = data
                pos = lines.find("\r\n")
                while pos > 0:
                    yield lines[:pos+2]
                    lines = lines[pos+2:]
                    pos = lines.find("\r\n")
                if len(lines):
                    yield lines
            func(RequestParser(line_at_a_time()))

            def byte_at_a_time():
                for d in data:
                    yield d
            func(RequestParser(byte_at_a_time()))

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

