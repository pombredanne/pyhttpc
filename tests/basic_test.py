import t
import pyhttpc

class TestParser(pyhttpc.Parser):

    def __init__(self):
        self.fields = {}
        self.was_field = False
        self.prev_field = None

    def on_message_begin(self):
        self.began = True

    def on_method(self, method):
        self.method = method;

    def on_path(self, path):
        self.path = path

    def on_query_string(self, qs):
        self.qs = qs

    def on_url(self, url):
        self.url = url

    def on_fragment(self, frag):
        self.fragment = frag

    def on_header_field(self, field):
        if self.was_field:
            self.prev_field += field
        else:
            self.prev_field = field
        self.was_field = True

    def on_header_value(self, value):
        if self.was_field:
            self.fields[self.prev_field] = value
        else:
            self.fields[self.prev_field] += value
        self.was_field = False
    
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
    Content-Type: application/json\r
    Content-Length: 14\r
    \r
    {"nom": "nom"}
    """
    p = TestParser()
    p.parse_requests(mesg)
    t.eq(p.began, True)
    t.eq(p.method, "PUT")
    t.eq(p.path, "/stuff/here")
    t.eq(p.qs, "foo=bar")
    t.eq(p.fragment, "baz")
    t.eq(p.fields, {
        "Server": "http://127.0.0.1:5984",
        "Content-Type": "application/json",
        "Content-Length": "14"
    })
    t.eq(p.body, '{"nom": "nom"}')
    t.eq(p.done, True)
    t.eq(p.keep_alive, False)


