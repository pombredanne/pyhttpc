import os
import t

import pyhttpc.native as native

dirname = os.path.dirname(__file__)

@t.request("001.http")
def test_001(parser):
    print "READ REQ"
    req = parser.next()
    t.eq(req.method, "PUT")
    t.eq(req.uri, "/stuff/here?foo=bar")
    t.eq(req.scheme, None)
    t.eq(req.host, None)
    t.eq(req.port, 80)
    t.eq(req.path, "/stuff/here")
    t.eq(req.query, "foo=bar")
    t.eq(req.fragment, None)
    t.eq(req.version, (1, 0))
    t.eq(req.headers, [
        ("Server", "http://127.0.0.1:5984"),
        ("Content-Type", "application/json"),
        ("Content-Length", "14")    
    ])
    t.eq(req.read(), '{"nom": "nom"}')
    print "DONE"