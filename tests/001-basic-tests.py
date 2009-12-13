import t

test_type = t.REQUEST
def test(): t.run_tests()

def req001(r):
    t.eq(r.method, "PUT")
    t.eq(r.version, (1, 0))
    t.eq(r.path, "/stuff/here")
    t.eq(r.query_string, "foo=bar")
    t.eq(sorted(r.headers), [
        ('Content-Length', '16\r\n'),
        ('Content-Type', 'application/json\r\n'),
        ('Server', 'http://127.0.0.1:5984\r\n')
    ])
    t.eq(r.read(), '{"nom": "nom"}\r\n')
