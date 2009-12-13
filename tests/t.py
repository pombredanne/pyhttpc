# Copyright 2009 Paul J. Davis <paul.joseph.davis@gmail.com>
#
# This file is part of the pywebmachine package released
# under the MIT license.
import inspect
import os
import re
import unittest

import pyhttpc

REQUEST = 1
RESPONSE = 2

def get_source():
    frames = inspect.stack()
    srcfile = frames[2][0].f_code.co_filename
    fname = os.path.splitext(srcfile)[0] + ".http"
    with open(fname) as handle:
        for line in handle:
            yield line.rstrip("\r\n") + "\r\n"

tre = re.compile(r"req\d+")
def get_tests():
    ret = []
    frames = inspect.stack()
    for g, mem in frames[2][0].f_globals.iteritems():
        if not inspect.isfunction(mem):
            continue
        if not tre.match(g):
            continue
        ret.append((g, mem))
    ret.sort()
    test_type = frames[2][0].f_globals.get("test_type", None)
    if test_type is None:
        raise ValueError("No 'test_type' specified.")
    return (test_type, ret)

def run_tests():
    source = get_source()
    (test_type, tests) = get_tests()
    if test_type is REQUEST:
        i = pyhttpc.parse_requests(source)
    else:
        raise ValueError("No responses yet...")
    for (name, test) in tests:
        test(i.next())
    raises(StopIteration, i.next)
    
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

