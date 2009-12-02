import t
import pyhttpc

@t.docs
def test_on_url(mesg):
    """\
    PUT /stuff/here?foo=bar HTTP/1.0\r
    Server: http://127.0.0.1:5984\r
    Content-Type: application/json\r
    Content-Length: 15\r
    \r
    {"nom": "nom"}
    """
    print '%r' % mesg
    p = pyhttpc.Parser()
    parsed = p.parse_requests(mesg)
    t.eq(len(mesg), parsed)
    t.eq(p.request.method, "PUT")
    t.eq(p.request.path, "/stuff/here")
    t.eq(p.request.query_string, "foo=bar")
    t.eq(p.request.body, '{"nom": "nom"}\n')
