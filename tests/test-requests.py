import t

@t.request("001.http")
def test_001(parser):
    r = parser.next()
    t.eq(r.method, "PUT")
    t.eq(r.version, (1, 0))
    t.eq(r.path, "/stuff/here")
    t.eq(r.query_string, "foo=bar")
    t.eq(sorted(r.headers), [
        ('Content-Length', '14\r\n'),
        ('Content-Type', 'application/json\r\n'),
        ('Server', 'http://127.0.0.1:5984\r\n')
    ])
    t.eq(r.read(), '{"nom": "nom"}')
    t.raises(StopIteration, parser.next)

@t.request("002.http")
def test_002(parser):
    r = parser.next()
    t.eq(r.method, "GET")
    t.eq(r.version, (1, 1))
    t.eq(r.path, "/test")
    t.eq(r.query_string, None)
    t.eq(sorted(r.headers), [
        ("Accept", "*/*\r\n"),
        ("Host", "0.0.0.0=5000\r\n"),
        ("User-Agent", "curl/7.18.0 (i486-pc-linux-gnu) libcurl/7.18.0 OpenSSL/0.9.8g zlib/1.2.3.3 libidn/1.1\r\n")
    ])
    t.eq(r.read(), "")
    t.raises(StopIteration, parser.next)

@t.request("003.http")
def test_003(parser):
    r = parser.next()
    t.eq(r.method, "GET")
    t.eq(r.version, (1, 1))
    t.eq(r.path, "/favicon.ico")
    t.eq(r.query_string, None)
    t.eq(sorted(r.headers), [
        ("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"),
        ("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"),
        ("Accept-Encoding", "gzip,deflate\r\n"),
        ("Accept-Language", "en-us,en;q=0.5\r\n"),
        ("Connection", "keep-alive\r\n"),
        ("Host", "0.0.0.0=5000\r\n"),
        ("Keep-Alive", "300\r\n"),
        ("User-Agent", "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) Gecko/2008061015 Firefox/3.0\r\n"),
    ])
    t.eq(r.read(), "")
    t.raises(StopIteration, parser.next)

@t.request("004.http")
def test_004(parser):
    r = parser.next()
    t.eq(r.method, "GET")
    t.eq(r.version, (1, 1))
    t.eq(r.path, "/dumbfuck")
    t.eq(r.query_string, None)
    t.eq(r.headers, [("aaaaaaaaaaaaa", "++++++++++\r\n")])
    t.eq(r.read(), "")
    t.raises(StopIteration, parser.next)

@t.request("005.http")
def test_005(parser):
    r = parser.next()
    t.eq(r.method, "GET")
    t.eq(r.version, (1, 1))
    t.eq(r.path, "/forums/1/topics/2375")
    t.eq(r.query_string, "page=1")
    t.eq(r.fragment, "posts-17408")
    t.eq(r.read(), "")
    t.raises(StopIteration, parser.next)

@t.request("006.http")
def test_006(parser):
    r = parser.next()
    t.eq(r.method, "GET")
    t.eq(r.version, (1, 1))
    t.eq(r.path, "/get_no_headers_no_body/world")
    t.eq(r.query_string, None)
    t.eq(r.fragment, None)
    t.eq(r.read(), "")
    t.raises(StopIteration, parser.next)

@t.request("007.http")
def test_007(parser):
    r = parser.next()
    t.eq(r.method, "GET")
    t.eq(r.version, (1, 1))
    t.eq(r.path, "/get_one_header_no_body")
    t.eq(r.query_string, None)
    t.eq(r.fragment, None)
    t.eq(r.headers, [('Accept', '*/*\r\n')])
    t.eq(r.read(), "")
    t.raises(StopIteration, parser.next)

@t.request("008.http")
def test_008(parser):
    r = parser.next()
    t.eq(r.method, "GET")
    t.eq(r.version, (1, 0))
    t.eq(r.path, "/get_funky_content_length_body_hello")
    t.eq(r.query_string, None)
    t.eq(r.fragment, None)
    t.eq(r.headers, [('conTENT-Length', '5\r\n')])
    t.eq(r.read(), "HELLO")
    t.raises(StopIteration, parser.next)

@t.request("009.http")
def test_009(parser):
    r = parser.next()
    t.eq(r.method, "POST")
    t.eq(r.version, (1, 1))
    t.eq(r.path, "/post_identity_body_world")
    t.eq(r.query_string, "q=search")
    t.eq(r.fragment, "hey")
    t.eq(sorted(r.headers), [
        ('Accept', '*/*\r\n'),
        ('Content-Length', '5\r\n'),
        ('Transfer-Encoding', 'identity\r\n')
    ])
    t.eq(r.read(), "World")
    t.eq(r.read(), "")
    t.raises(StopIteration, parser.next)



