try:
    from pyhttpc.native import RequestParser
except ImportError:
    raise
    #from pyhttpc.python import RequestParser

class RequestReader(object):
    def __init__(self, source):
        self.parser = RequestParser()
        self.source = source
    
    def read(self):
        req = None
        while req is None:
            req = self.parser.read(self.source.next())
        return req
