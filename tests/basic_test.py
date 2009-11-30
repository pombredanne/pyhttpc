import t
import pyhttpc

class TestParser(pyhttpc.Parser):

    def __init__(self):
        self.fields = {}

    def on_message_begin(self):
        self.began = True

    def on_path(self, path):
        self.path = path

    def on_query_string(self, qs):
        self.qs = qs

    def on_url(self, url):
        self.url = url

    def on_fragment(self, frag):
        self.fragment = frag

    def on_header_field(self, field):
        self.fields[field] = True

    def on_header_value(self, value):
        pass
    
    def on_headers_complete(self):
        self.headers_complete = True

    def on_body(self, body):
        self.body = body

    def on_message_complete(self):
        self.keep_alive = self.should_keep_alive()
        self.done = True

@t.docs
def test_on_url(mesg):
    """\
    PUT /stuff/here?foo=bar#baz HTTP/1.0\r
    Server: http://127.0.0.1:5984\r
    If-Match: "foobar" "foobar2"\r
    If-None-Match: "baz"\r
    If-None-Match: "bam"\r
    Content-Type: application/json\r
    Content-Length: 14\r
    \r
    {"nom": "nom"}
    """
    p = TestParser()
    print p.parse_requests(mesg)
    t.eq(p.began, True)
    t.eq(p.path, "/stuff/here")
    t.eq(p.qs, "foo=bar")
    t.eq(p.fragment, "baz")
    t.eq(p.fields, {"If-Match": True, "If-None-Match": True,
        "Server": True, "Content-Type": True, "Content-Length": True})
    t.eq(p.body, '{"nom": "nom"}')
    t.eq(p.done, True)
    t.eq(p.keep_alive, False)


