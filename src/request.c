
#line 1 "./src/request.rl"

#include <assert.h>
#include <string.h>

#include "request.h"


#line 103 "./src/request.rl"



#line 15 "./src/request.c"
static const char _http_req_parser_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 5, 1, 8, 1, 9, 1, 
	10, 1, 12, 1, 14, 1, 16, 1, 
	17, 1, 18, 1, 19, 2, 0, 8, 
	2, 3, 0, 2, 3, 10, 2, 4, 
	0, 2, 4, 5, 2, 4, 10, 2, 
	6, 0, 2, 6, 10, 2, 7, 10, 
	2, 9, 0, 2, 9, 10, 2, 11, 
	12, 2, 13, 14, 2, 15, 16, 2, 
	17, 18, 3, 3, 0, 8, 3, 4, 
	0, 8, 3, 6, 0, 8, 3, 7, 
	0, 8, 3, 9, 0, 8
};

static const short _http_req_parser_key_offsets[] = {
	0, 0, 8, 17, 18, 30, 31, 44, 
	57, 58, 60, 61, 62, 63, 64, 66, 
	69, 71, 74, 75, 77, 78, 94, 95, 
	111, 112, 113, 119, 125, 126, 136, 137, 
	138, 154, 159, 174, 180, 186, 200, 206, 
	212, 218, 224, 230, 236, 247, 258, 269, 
	278, 287, 296, 305, 314, 323, 332, 341, 
	350, 359, 368, 377, 386, 395, 404, 413, 
	422, 431, 440, 441
};

static const char _http_req_parser_trans_keys[] = {
	36, 95, 45, 46, 48, 57, 65, 90, 
	32, 36, 95, 45, 46, 48, 57, 65, 
	90, 9, 32, 35, 42, 43, 47, 63, 
	45, 57, 65, 90, 97, 122, 9, 32, 
	35, 42, 43, 47, 63, 72, 45, 57, 
	65, 90, 97, 122, 32, 33, 37, 61, 
	95, 36, 42, 44, 59, 64, 90, 97, 
	122, 9, 32, 72, 84, 84, 80, 47, 
	48, 57, 46, 48, 57, 48, 57, 32, 
	48, 57, 9, 13, 32, 10, 13, 33, 
	124, 126, 35, 39, 42, 43, 45, 46, 
	48, 57, 65, 90, 94, 122, 10, 33, 
	58, 124, 126, 35, 39, 42, 43, 45, 
	46, 48, 57, 65, 90, 94, 122, 13, 
	13, 48, 57, 65, 70, 97, 102, 48, 
	57, 65, 70, 97, 102, 32, 43, 58, 
	45, 46, 48, 57, 65, 90, 97, 122, 
	47, 47, 33, 37, 59, 61, 63, 95, 
	36, 42, 44, 46, 48, 57, 65, 90, 
	97, 122, 32, 35, 47, 58, 63, 32, 
	33, 35, 37, 61, 63, 95, 36, 42, 
	44, 59, 64, 90, 97, 122, 48, 57, 
	65, 70, 97, 102, 48, 57, 65, 70, 
	97, 102, 32, 33, 35, 37, 61, 95, 
	36, 42, 44, 59, 64, 90, 97, 122, 
	48, 57, 65, 70, 97, 102, 48, 57, 
	65, 70, 97, 102, 32, 35, 47, 63, 
	48, 57, 32, 35, 47, 63, 48, 57, 
	48, 57, 65, 70, 97, 102, 48, 57, 
	65, 70, 97, 102, 43, 58, 84, 45, 
	46, 48, 57, 65, 90, 97, 122, 43, 
	58, 84, 45, 46, 48, 57, 65, 90, 
	97, 122, 43, 58, 80, 45, 46, 48, 
	57, 65, 90, 97, 122, 43, 47, 58, 
	45, 57, 65, 90, 97, 122, 32, 36, 
	95, 45, 46, 48, 57, 65, 90, 32, 
	36, 95, 45, 46, 48, 57, 65, 90, 
	32, 36, 95, 45, 46, 48, 57, 65, 
	90, 32, 36, 95, 45, 46, 48, 57, 
	65, 90, 32, 36, 95, 45, 46, 48, 
	57, 65, 90, 32, 36, 95, 45, 46, 
	48, 57, 65, 90, 32, 36, 95, 45, 
	46, 48, 57, 65, 90, 32, 36, 95, 
	45, 46, 48, 57, 65, 90, 32, 36, 
	95, 45, 46, 48, 57, 65, 90, 32, 
	36, 95, 45, 46, 48, 57, 65, 90, 
	32, 36, 95, 45, 46, 48, 57, 65, 
	90, 32, 36, 95, 45, 46, 48, 57, 
	65, 90, 32, 36, 95, 45, 46, 48, 
	57, 65, 90, 32, 36, 95, 45, 46, 
	48, 57, 65, 90, 32, 36, 95, 45, 
	46, 48, 57, 65, 90, 32, 36, 95, 
	45, 46, 48, 57, 65, 90, 32, 36, 
	95, 45, 46, 48, 57, 65, 90, 32, 
	36, 95, 45, 46, 48, 57, 65, 90, 
	32, 0
};

static const char _http_req_parser_single_lengths[] = {
	0, 2, 3, 1, 6, 1, 7, 5, 
	1, 2, 1, 1, 1, 1, 0, 1, 
	0, 1, 1, 2, 1, 4, 1, 4, 
	1, 1, 0, 0, 1, 2, 1, 1, 
	6, 5, 7, 0, 0, 6, 0, 0, 
	4, 4, 0, 0, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 1, 0
};

static const char _http_req_parser_range_lengths[] = {
	0, 3, 3, 0, 3, 0, 3, 4, 
	0, 0, 0, 0, 0, 0, 1, 1, 
	1, 1, 0, 0, 0, 6, 0, 6, 
	0, 0, 3, 3, 0, 4, 0, 0, 
	5, 0, 4, 3, 3, 4, 3, 3, 
	1, 1, 3, 3, 4, 4, 4, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 0, 0
};

static const short _http_req_parser_index_offsets[] = {
	0, 0, 6, 13, 15, 25, 27, 38, 
	48, 50, 53, 55, 57, 59, 61, 63, 
	66, 68, 71, 73, 76, 78, 89, 91, 
	102, 104, 106, 110, 114, 116, 123, 125, 
	127, 139, 145, 157, 161, 165, 176, 180, 
	184, 190, 196, 200, 204, 212, 220, 228, 
	235, 242, 249, 256, 263, 270, 277, 284, 
	291, 298, 305, 312, 319, 326, 333, 340, 
	347, 354, 361, 363
};

static const char _http_req_parser_indicies[] = {
	0, 0, 0, 0, 0, 1, 2, 3, 
	3, 3, 3, 3, 1, 4, 1, 5, 
	6, 7, 8, 9, 10, 8, 8, 8, 
	1, 11, 1, 5, 6, 7, 8, 9, 
	10, 12, 8, 8, 8, 1, 13, 14, 
	15, 14, 14, 14, 14, 14, 14, 1, 
	16, 1, 17, 18, 1, 19, 1, 20, 
	1, 21, 1, 22, 1, 23, 1, 24, 
	25, 1, 26, 1, 27, 28, 1, 29, 
	1, 30, 27, 1, 31, 1, 32, 33, 
	33, 33, 33, 33, 33, 33, 33, 33, 
	1, 34, 1, 35, 36, 35, 35, 35, 
	35, 35, 35, 35, 35, 1, 38, 37, 
	30, 39, 40, 40, 40, 1, 14, 14, 
	14, 1, 13, 1, 41, 42, 41, 41, 
	41, 41, 1, 43, 1, 44, 1, 45, 
	46, 45, 45, 45, 45, 45, 45, 45, 
	45, 45, 1, 47, 48, 49, 50, 51, 
	1, 52, 53, 54, 55, 53, 56, 53, 
	53, 53, 53, 53, 1, 57, 57, 57, 
	1, 53, 53, 53, 1, 58, 59, 60, 
	61, 59, 59, 59, 59, 59, 59, 1, 
	62, 62, 62, 1, 59, 59, 59, 1, 
	63, 64, 65, 67, 66, 1, 13, 68, 
	69, 71, 70, 1, 72, 72, 72, 1, 
	73, 73, 73, 1, 41, 42, 74, 41, 
	41, 41, 41, 1, 41, 42, 75, 41, 
	41, 41, 41, 1, 41, 42, 76, 41, 
	41, 41, 41, 1, 41, 22, 42, 41, 
	41, 41, 1, 2, 77, 77, 77, 77, 
	77, 1, 2, 78, 78, 78, 78, 78, 
	1, 2, 79, 79, 79, 79, 79, 1, 
	2, 80, 80, 80, 80, 80, 1, 2, 
	81, 81, 81, 81, 81, 1, 2, 82, 
	82, 82, 82, 82, 1, 2, 83, 83, 
	83, 83, 83, 1, 2, 84, 84, 84, 
	84, 84, 1, 2, 85, 85, 85, 85, 
	85, 1, 2, 86, 86, 86, 86, 86, 
	1, 2, 87, 87, 87, 87, 87, 1, 
	2, 88, 88, 88, 88, 88, 1, 2, 
	89, 89, 89, 89, 89, 1, 2, 90, 
	90, 90, 90, 90, 1, 2, 91, 91, 
	91, 91, 91, 1, 2, 92, 92, 92, 
	92, 92, 1, 2, 93, 93, 93, 93, 
	93, 1, 2, 94, 94, 94, 94, 94, 
	1, 2, 1, 1, 0
};

static const char _http_req_parser_trans_targs[] = {
	2, 0, 3, 48, 4, 5, 7, 28, 
	29, 34, 37, 6, 44, 8, 7, 26, 
	9, 8, 10, 11, 12, 13, 14, 15, 
	16, 15, 17, 18, 17, 19, 20, 21, 
	22, 23, 67, 23, 24, 25, 20, 25, 
	27, 29, 30, 31, 32, 33, 42, 8, 
	7, 34, 40, 37, 8, 34, 7, 35, 
	37, 36, 8, 37, 7, 38, 39, 8, 
	7, 34, 41, 37, 7, 34, 41, 37, 
	43, 33, 45, 46, 47, 49, 50, 51, 
	52, 53, 54, 55, 56, 57, 58, 59, 
	60, 61, 62, 63, 64, 65, 66
};

static const char _http_req_parser_trans_actions[] = {
	1, 0, 3, 0, 0, 59, 90, 13, 
	56, 56, 56, 0, 56, 15, 11, 11, 
	0, 0, 0, 0, 0, 0, 0, 62, 
	0, 17, 65, 0, 19, 0, 0, 0, 
	0, 68, 27, 21, 0, 71, 23, 25, 
	11, 0, 5, 0, 0, 1, 1, 35, 
	74, 32, 7, 32, 50, 0, 82, 0, 
	47, 0, 53, 0, 86, 0, 0, 44, 
	78, 38, 41, 38, 29, 1, 9, 1, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
};

static const int http_req_parser_start = 1;
static const int http_req_parser_first_final = 67;
static const int http_req_parser_error = 0;

static const int http_req_parser_en_main = 1;


#line 106 "./src/request.rl"

void
write_field(RequestParser* parser, char** field, const char* ptr)
{

}

void
write_uri(RequestParser* parser, const char* ptr)
{

}

void
add_header(RequestParser* parser)
{

}

void
save_mark_buf(RequestParser* parser)
{

}

void
save_mark_uri_buf(RequestParser* parser)
{

}

int
int_req_parser(RequestParser *parser, PyObject* source)
{
    int cs = 0;
    
#line 261 "./src/request.c"
	{
	cs = http_req_parser_start;
	}

#line 142 "./src/request.rl"

    parser->cs = cs;
    parser->error = 0;

    parser->body = NULL;
    parser->nread = 0;
    
    parser->method = NULL;
    parser->uri = NULL;
    parser->scheme = NULL;
    parser->host = NULL;
    parser->port = 80;
    parser->path = NULL;
    parser->query = NULL;
    parser->fragment = NULL;
    
    parser->vsn_major = 0;
    parser->vsn_minor = 0;

    parser->headers = NULL;
    parser->hdr_name = NULL;
    parser->hdr_value = NULL;

    parser->mark = NULL;
    parser->mark_buf = NULL;
    parser->mark_len = 0;
    
    parser->mark_uri = NULL;
    parser->mark_uri_buf = NULL;
    parser->mark_uri_len = 0;

    return(1);
}

size_t
exec_req_parser(RequestParser* parser, const char* buffer, size_t len)
{
    const char* p;
    const char* pe;
    int cs = parser->cs;
    
    p = buffer;
    pe = buffer + len;
    
    
#line 312 "./src/request.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _http_req_parser_trans_keys + _http_req_parser_key_offsets[cs];
	_trans = _http_req_parser_index_offsets[cs];

	_klen = _http_req_parser_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _http_req_parser_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += ((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _http_req_parser_indicies[_trans];
	cs = _http_req_parser_trans_targs[_trans];

	if ( _http_req_parser_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _http_req_parser_actions + _http_req_parser_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 10 "./src/request.rl"
	{
        assert(parser->mark == NULL && "won't overwrite a mark.");
        parser->mark = p;
    }
	break;
	case 1:
#line 15 "./src/request.rl"
	{
        write_field(parser, &(parser->method), p);
    }
	break;
	case 2:
#line 19 "./src/request.rl"
	{
        write_field(parser, &(parser->scheme), p);
    }
	break;
	case 3:
#line 23 "./src/request.rl"
	{
        write_field(parser, &(parser->host), p);
    }
	break;
	case 4:
#line 27 "./src/request.rl"
	{
        parser->port = 0;
    }
	break;
	case 5:
#line 31 "./src/request.rl"
	{
        parser->port = parser->port*10 + ((*p)-'0');
    }
	break;
	case 6:
#line 35 "./src/request.rl"
	{
        write_field(parser, &(parser->path), p);
    }
	break;
	case 7:
#line 39 "./src/request.rl"
	{
        write_field(parser, &(parser->query), p);
    }
	break;
	case 8:
#line 43 "./src/request.rl"
	{
        write_field(parser, &(parser->fragment), p);
    }
	break;
	case 9:
#line 47 "./src/request.rl"
	{
        assert(parser->mark_uri != NULL && "wont overwrite uri mark");
        parser->mark_uri = p;
    }
	break;
	case 10:
#line 52 "./src/request.rl"
	{
        write_uri(parser, p);
    }
	break;
	case 11:
#line 56 "./src/request.rl"
	{
        parser->vsn_major = 0;
    }
	break;
	case 12:
#line 60 "./src/request.rl"
	{
        parser->vsn_major = parser->vsn_major*10 + ((*p)-'0');
    }
	break;
	case 13:
#line 64 "./src/request.rl"
	{
        parser->vsn_minor = 0;
    }
	break;
	case 14:
#line 68 "./src/request.rl"
	{
        parser->vsn_minor = parser->vsn_minor * 10 + ((*p)-'0');
    }
	break;
	case 15:
#line 72 "./src/request.rl"
	{
        assert(parser->method != NULL && "method not set before header name");
        assert(parser->uri != NULL && "uri not set before header name");
        assert(parser->hdr_field == NULL && "header name already marked");
        assert(parser->hdr_value == NULL && "header value already marked");
        
        assert(parser->mark == NULL && "wont overwrite a mark");
        parser->mark = p;
    }
	break;
	case 16:
#line 82 "./src/request.rl"
	{
        write_field(parser, &(parser->hdr_name), p);
    }
	break;
	case 17:
#line 86 "./src/request.rl"
	{
        assert(parser->hdr_name != NULL && "value must have a name");
        parser->mark = p;
    }
	break;
	case 18:
#line 91 "./src/request.rl"
	{
        write_field(parser, &(parser->hdr_value), p);
        add_header(parser);
    }
	break;
	case 19:
#line 96 "./src/request.rl"
	{
        parser->body = p;
    }
	break;
#line 516 "./src/request.c"
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 187 "./src/request.rl"
    
    parser->cs = cs;
    parser->nread += p - buffer;
    
    assert(p < pe && "parser boundary error");
    
    if(parser->mark != NULL)
    {
        save_mark_buf(parser);
    }
    
    if(parser->mark_uri != NULL)
    {
        save_mark_uri_buf(parser);
    }
    
    return parser->nread;    
}
