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

@t.request("010.http")
def test_010(parser):
    r = parser.next()
    t.eq(r.method, "POST")
    t.eq(r.version, (1, 1))
    t.eq(r.path, "/post_chunked_all_your_base")
    t.eq(r.query_string, None)
    t.eq(r.fragment, None)
    t.eq(sorted(r.headers), [
        ("Transfer-Encoding", "chunked\r\n")
    ])
    t.eq(r.read(), "all your base are belong to us")
    t.eq(r.read(), "")
    t.raises(StopIteration, parser.next)

@t.request("011.http")
def test_011(parser):
    r = parser.next()
    t.eq(r.method, "POST")
    t.eq(r.version, (1, 1))
    t.eq(r.path, "/two_chunks_mult_zero_end")
    t.eq(r.query_string, None)
    t.eq(r.fragment, None)
    t.eq(sorted(r.headers), [
        ("Transfer-Encoding", "chunked\r\n")
    ])
    t.eq(r.read(), "hello world")
    t.eq(r.read(), "")
    t.raises(StopIteration, parser.next)

# Need to add trailer support.
#@t.request("012.http")
#def test_012(parser):
#    r = parser.next()
#    t.eq(r.method, "POST")
#    t.eq(r.version, (1, 1))
#    t.eq(r.path, "/chunked_w_trailing_headers")
#    t.eq(r.query_string, None)
#    t.eq(r.fragment, None)
#    t.eq(sorted(r.headers), [
#        ("Transfer-Encoding", "chunked\r\n")
#    ])
#    t.eq(r.read(), "hello world")

@t.request("013.http")
def test_013(parser):
    r = parser.next()
    t.eq(r.method, "POST")
    t.eq(r.version, (1, 1))
    t.eq(r.path, "/chunked_w_bullshit_after_length")
    t.eq(r.query_string, None)
    t.eq(r.fragment, None)
    t.eq(sorted(r.headers), [
        ("Transfer-Encoding", "chunked\r\n")
    ])
    t.eq(r.read(), "hello world")
    t.eq(r.read(), "")
    t.raises(StopIteration, parser.next)
    
@t.request("014.http")
def test_014(parser):
    r = parser.next()
    t.eq(r.method, "GET")
    t.eq(r.version, (1, 1))
    t.eq(r.path, '/with_"stupid"_quotes')
    t.eq(r.query_string, 'foo="bar"')
    t.eq(r.fragment, None)
    t.eq(r.read(), "")
    t.raises(StopIteration, parser.next)

@t.request("015.http")
def test_015(parser):
    r = parser.next()
    t.eq(r.method, "GET")
    t.eq(r.version, (1, 0))
    t.eq(r.path, "/test")
    t.eq(r.query_string, None)
    t.eq(r.fragment, None)
    t.eq(sorted(r.headers), [
        ("Accept", "*/*\r\n"),
        ("Host", "0.0.0.0:5000\r\n"),
        ("User-Agent", "ApacheBench/2.3\r\n")
    ])
    t.eq(r.read(), "")
    t.raises(StopIteration, parser.next)

@t.request("016.http")
def test_016(parser):
    r = parser.next()
    t.eq(r.method, "GET")
    t.eq(r.version, (1, 1))
    t.eq(r.path, "/")
    t.eq(r.query_string, None)
    t.eq(r.fragment, None)
    t.eq(sorted(r.headers), [
        ("X-SSL-Bullshit",
        """-----BEGIN CERTIFICATE-----\r
    MIIFbTCCBFWgAwIBAgICH4cwDQYJKoZIhvcNAQEFBQAwcDELMAkGA1UEBhMCVUsx\r
    ETAPBgNVBAoTCGVTY2llbmNlMRIwEAYDVQQLEwlBdXRob3JpdHkxCzAJBgNVBAMT\r
    AkNBMS0wKwYJKoZIhvcNAQkBFh5jYS1vcGVyYXRvckBncmlkLXN1cHBvcnQuYWMu\r
    dWswHhcNMDYwNzI3MTQxMzI4WhcNMDcwNzI3MTQxMzI4WjBbMQswCQYDVQQGEwJV\r
    SzERMA8GA1UEChMIZVNjaWVuY2UxEzARBgNVBAsTCk1hbmNoZXN0ZXIxCzAJBgNV\r
    BAcTmrsogriqMWLAk1DMRcwFQYDVQQDEw5taWNoYWVsIHBhcmQYJKoZIhvcNAQEB\r
    BQADggEPADCCAQoCggEBANPEQBgl1IaKdSS1TbhF3hEXSl72G9J+WC/1R64fAcEF\r
    W51rEyFYiIeZGx/BVzwXbeBoNUK41OK65sxGuflMo5gLflbwJtHBRIEKAfVVp3YR\r
    gW7cMA/s/XKgL1GEC7rQw8lIZT8RApukCGqOVHSi/F1SiFlPDxuDfmdiNzL31+sL\r
    0iwHDdNkGjy5pyBSB8Y79dsSJtCW/iaLB0/n8Sj7HgvvZJ7x0fr+RQjYOUUfrePP\r
    u2MSpFyf+9BbC/aXgaZuiCvSR+8Snv3xApQY+fULK/xY8h8Ua51iXoQ5jrgu2SqR\r
    wgA7BUi3G8LFzMBl8FRCDYGUDy7M6QaHXx1ZWIPWNKsCAwEAAaOCAiQwggIgMAwG\r
    1UdEwEB/wQCMAAwEQYJYIZIAYb4QgHTTPAQDAgWgMA4GA1UdDwEB/wQEAwID6DAs\r
    BglghkgBhvhCAQ0EHxYdVUsgZS1TY2llbmNlIFVzZXIgQ2VydGlmaWNhdGUwHQYD\r
    VR0OBBYEFDTt/sf9PeMaZDHkUIldrDYMNTBZMIGaBgNVHSMEgZIwgY+AFAI4qxGj\r
    loCLDdMVKwiljjDastqooXSkcjBwMQswCQYDVQQGEwJVSzERMA8GA1UEChMIZVNj\r
    aWVuY2UxEjAQBgNVBAsTCUF1dGhvcml0eTELMAkGA1UEAxMCQ0ExLTArBgkqhkiG\r
    9w0BCQEWHmNhLW9wZXJhdG9yQGdyaWQtc3VwcG9ydC5hYy51a4IBADApBgNVHRIE\r
    IjAggR5jYS1vcGVyYXRvckBncmlkLXN1cHBvcnQuYWMudWswGQYDVR0gBBIwEDAO\r
    BgwrBgEEAdkvAQEBAQYwPQYJYIZIAYb4QgEEBDAWLmh0dHA6Ly9jYS5ncmlkLXN1\r
    cHBvcnQuYWMudmT4sopwqlBWsvcHViL2NybC9jYWNybC5jcmwwPQYJYIZIAYb4Qg\r
    EDBDAWLmh0dHA6Ly9jYS5ncmlkLXN1cHBvcnQuYWMudWsvcHViL2NybC9jYWNybC\r
    5jcmwwPwYDVR0fBDgwNjA0oDKgMIYuaHR0cDovL2NhLmdyaWQt5hYy51ay9wdWIv\r
    Y3JsL2NhY3JsLmNybDANBgkqhkiG9w0BAQUFAAOCAQEAS/U4iiooBENGW/Hwmmd3\r
    XCy6Zrt08YjKCzGNjorT98g8uGsqYjSxv/hmi0qlnlHs+k/3Iobc3LjS5AMYr5L8\r
    UO7OSkgFFlLHQyC9JzPfmLCAugvzEbyv4Olnsr8hbxF1MbKZoQxUZtMVu29wjfXk\r
    hTeApBv7eaKCWpSp7MCbvgzm74izKhu3vlDk9w6qVrxePfGgpKPqfHiOoGhFnbTK\r
    wTC6o2xq5y0qZ03JonF7OJspEd3I5zKY3E+ov7/ZhW6DqT8UFvsAdjvQbXyhV8Eu\r
    Yhixw1aKEPzNjNowuIseVogKOLXxWI5vAi5HgXdS0/ES5gDGsABo4fqovUKlgop3\r
    RA==\r
    -----END CERTIFICATE-----\r\n""")
    ])
    t.eq(r.read(), "")