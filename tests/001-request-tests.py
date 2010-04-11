import t

@t.request("001.http")
def test_001(p, data):
    req = None
    for idx, d in enumerate(data):
        req = p.read(d)
        if req:
            break
    exp = {
        "method": "PUT",
        "uri": "/stuff/here?foo=bar",
        "scheme": None,
        "host": None,
        "port": 80,
        "path": "/stuff/here",
        "query": "foo=bar",
        "fragment": None,
        "version": (1, 0),
        "headers": [
            ("Server", "http://127.0.0.1:5984"),
            ("Content-Type", "application/json"),
            ("Content-Length", "14")
        ],
    }
    t.eq(req, exp)
    b = p.body() + "".join(data[idx:])
    t.eq(b, '{"nom": "nom"}')