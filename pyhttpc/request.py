try:
    from pyhttpc.native import RequestParser
except ImportError:
    raise
    #from pyhttpc.python import RequestParser

try:
    from cStringIO import StringIO
except ImportError:
    from StringIO import StringIO

class RequestReader(object):
    def __init__(self):
        pass
