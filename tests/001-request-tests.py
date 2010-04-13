import t

import pyhttpc.native as native

def test_basic():
    def get_data():
        yield "foo"
    
    parser = native.RequestParser(get_data())
    req = parser.next()
    t.eq(isinstance(req, native.Request), True)
    t.eq(req.method, "GET")


# @t.request("001.http")
# def test_001(reader):
#     req = reader.read()
#     exp = {
#         "method": "PUT",
#         "uri": "/stuff/here?foo=bar",
#         "scheme": None,
#         "host": None,
#         "port": 80,
#         "path": "/stuff/here",
#         "query": "foo=bar",
#         "fragment": None,
#         "version": (1, 0),
#         "headers": [
#             ("Server", "http://127.0.0.1:5984"),
#             ("Content-Type", "application/json"),
#             ("Content-Length", "14")
#         ],
#     }
#     t.eq(req, exp)
#     t.eq(req.read(), '{"nom": "nom"}')