
#line 1 "./c_src/request.rl"

#include "request.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define WHERE fprintf(stderr, "%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__)


#line 94 "./c_src/request.rl"



#line 18 "./c_src/request.c"
static const char _http_req_parser_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	8, 1, 9, 1, 10, 1, 12, 1, 
	14, 1, 15, 1, 16, 1, 17, 1, 
	18, 1, 19, 2, 0, 8, 2, 3, 
	0, 2, 3, 10, 2, 4, 0, 2, 
	4, 5, 2, 4, 10, 2, 6, 10, 
	2, 7, 10, 2, 9, 0, 2, 9, 
	10, 2, 11, 12, 2, 13, 14, 2, 
	17, 18, 3, 0, 7, 8, 3, 0, 
	7, 10, 3, 3, 0, 8, 3, 4, 
	0, 8, 3, 6, 0, 8, 3, 7, 
	0, 8, 3, 9, 0, 8
};

static const short _http_req_parser_key_offsets[] = {
	0, 0, 8, 18, 31, 45, 59, 62, 
	63, 64, 65, 66, 68, 71, 73, 78, 
	81, 82, 98, 99, 115, 118, 119, 125, 
	131, 133, 143, 144, 145, 161, 167, 183, 
	189, 195, 210, 225, 231, 237, 244, 251, 
	257, 263, 274, 285, 296, 305, 315, 325, 
	335, 345, 355, 365, 375, 385, 395, 405, 
	415, 425, 435, 445, 455, 465, 475, 485, 
	487
};

static const char _http_req_parser_trans_keys[] = {
	36, 95, 45, 46, 48, 57, 65, 90, 
	9, 32, 36, 95, 45, 46, 48, 57, 
	65, 90, 9, 32, 35, 42, 43, 47, 
	63, 45, 57, 65, 90, 97, 122, 9, 
	32, 35, 42, 43, 47, 63, 72, 45, 
	57, 65, 90, 97, 122, 9, 32, 33, 
	37, 61, 95, 36, 42, 44, 59, 64, 
	90, 97, 122, 9, 32, 72, 84, 84, 
	80, 47, 48, 57, 46, 48, 57, 48, 
	57, 9, 13, 32, 48, 57, 9, 13, 
	32, 10, 13, 33, 124, 126, 35, 39, 
	42, 43, 45, 46, 48, 57, 65, 90, 
	94, 122, 10, 33, 58, 124, 126, 35, 
	39, 42, 43, 45, 46, 48, 57, 65, 
	90, 94, 122, 9, 13, 32, 13, 48, 
	57, 65, 70, 97, 102, 48, 57, 65, 
	70, 97, 102, 9, 32, 43, 58, 45, 
	46, 48, 57, 65, 90, 97, 122, 47, 
	47, 33, 37, 59, 61, 63, 95, 36, 
	42, 44, 46, 48, 57, 65, 90, 97, 
	122, 9, 32, 35, 47, 58, 63, 9, 
	32, 33, 35, 37, 61, 63, 95, 36, 
	42, 44, 59, 64, 90, 97, 122, 48, 
	57, 65, 70, 97, 102, 48, 57, 65, 
	70, 97, 102, 9, 32, 33, 35, 37, 
	61, 95, 36, 42, 44, 59, 64, 90, 
	97, 122, 9, 32, 33, 35, 37, 61, 
	95, 36, 42, 44, 59, 64, 90, 97, 
	122, 48, 57, 65, 70, 97, 102, 48, 
	57, 65, 70, 97, 102, 9, 32, 35, 
	47, 63, 48, 57, 9, 32, 35, 47, 
	63, 48, 57, 48, 57, 65, 70, 97, 
	102, 48, 57, 65, 70, 97, 102, 43, 
	58, 84, 45, 46, 48, 57, 65, 90, 
	97, 122, 43, 58, 84, 45, 46, 48, 
	57, 65, 90, 97, 122, 43, 58, 80, 
	45, 46, 48, 57, 65, 90, 97, 122, 
	43, 47, 58, 45, 57, 65, 90, 97, 
	122, 9, 32, 36, 95, 45, 46, 48, 
	57, 65, 90, 9, 32, 36, 95, 45, 
	46, 48, 57, 65, 90, 9, 32, 36, 
	95, 45, 46, 48, 57, 65, 90, 9, 
	32, 36, 95, 45, 46, 48, 57, 65, 
	90, 9, 32, 36, 95, 45, 46, 48, 
	57, 65, 90, 9, 32, 36, 95, 45, 
	46, 48, 57, 65, 90, 9, 32, 36, 
	95, 45, 46, 48, 57, 65, 90, 9, 
	32, 36, 95, 45, 46, 48, 57, 65, 
	90, 9, 32, 36, 95, 45, 46, 48, 
	57, 65, 90, 9, 32, 36, 95, 45, 
	46, 48, 57, 65, 90, 9, 32, 36, 
	95, 45, 46, 48, 57, 65, 90, 9, 
	32, 36, 95, 45, 46, 48, 57, 65, 
	90, 9, 32, 36, 95, 45, 46, 48, 
	57, 65, 90, 9, 32, 36, 95, 45, 
	46, 48, 57, 65, 90, 9, 32, 36, 
	95, 45, 46, 48, 57, 65, 90, 9, 
	32, 36, 95, 45, 46, 48, 57, 65, 
	90, 9, 32, 36, 95, 45, 46, 48, 
	57, 65, 90, 9, 32, 36, 95, 45, 
	46, 48, 57, 65, 90, 9, 32, 0
};

static const char _http_req_parser_single_lengths[] = {
	0, 2, 4, 7, 8, 6, 3, 1, 
	1, 1, 1, 0, 1, 0, 3, 3, 
	1, 4, 1, 4, 3, 1, 0, 0, 
	2, 2, 1, 1, 6, 6, 8, 0, 
	0, 7, 7, 0, 0, 5, 5, 0, 
	0, 3, 3, 3, 3, 4, 4, 4, 
	4, 4, 4, 4, 4, 4, 4, 4, 
	4, 4, 4, 4, 4, 4, 4, 2, 
	0
};

static const char _http_req_parser_range_lengths[] = {
	0, 3, 3, 3, 3, 4, 0, 0, 
	0, 0, 0, 1, 1, 1, 1, 0, 
	0, 6, 0, 6, 0, 0, 3, 3, 
	0, 4, 0, 0, 5, 0, 4, 3, 
	3, 4, 4, 3, 3, 1, 1, 3, 
	3, 4, 4, 4, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 0, 
	0
};

static const short _http_req_parser_index_offsets[] = {
	0, 0, 6, 14, 25, 37, 48, 52, 
	54, 56, 58, 60, 62, 65, 67, 72, 
	76, 78, 89, 91, 102, 106, 108, 112, 
	116, 119, 126, 128, 130, 142, 149, 162, 
	166, 170, 182, 194, 198, 202, 209, 216, 
	220, 224, 232, 240, 248, 255, 263, 271, 
	279, 287, 295, 303, 311, 319, 327, 335, 
	343, 351, 359, 367, 375, 383, 391, 399, 
	402
};

static const char _http_req_parser_indicies[] = {
	0, 0, 0, 0, 0, 1, 2, 2, 
	3, 3, 3, 3, 3, 1, 4, 4, 
	5, 6, 7, 8, 9, 7, 7, 7, 
	1, 4, 4, 5, 6, 7, 8, 9, 
	10, 7, 7, 7, 1, 11, 11, 12, 
	13, 12, 12, 12, 12, 12, 12, 1, 
	14, 14, 15, 1, 16, 1, 17, 1, 
	18, 1, 19, 1, 20, 1, 21, 22, 
	1, 23, 1, 24, 25, 24, 26, 1, 
	24, 25, 24, 1, 27, 1, 28, 29, 
	29, 29, 29, 29, 29, 29, 29, 29, 
	1, 30, 1, 31, 32, 31, 31, 31, 
	31, 31, 31, 31, 31, 1, 34, 35, 
	34, 33, 37, 36, 38, 38, 38, 1, 
	12, 12, 12, 1, 11, 11, 1, 39, 
	40, 39, 39, 39, 39, 1, 41, 1, 
	42, 1, 43, 44, 43, 43, 43, 43, 
	43, 43, 43, 43, 43, 1, 45, 45, 
	46, 47, 48, 49, 1, 50, 50, 51, 
	52, 53, 51, 54, 51, 51, 51, 51, 
	51, 1, 55, 55, 55, 1, 51, 51, 
	51, 1, 56, 56, 57, 58, 59, 57, 
	57, 57, 57, 57, 57, 1, 60, 60, 
	61, 62, 63, 61, 61, 61, 61, 61, 
	61, 1, 64, 64, 64, 1, 61, 61, 
	61, 1, 65, 65, 66, 67, 69, 68, 
	1, 11, 11, 70, 71, 73, 72, 1, 
	74, 74, 74, 1, 75, 75, 75, 1, 
	39, 40, 76, 39, 39, 39, 39, 1, 
	39, 40, 77, 39, 39, 39, 39, 1, 
	39, 40, 78, 39, 39, 39, 39, 1, 
	39, 19, 40, 39, 39, 39, 1, 2, 
	2, 79, 79, 79, 79, 79, 1, 2, 
	2, 80, 80, 80, 80, 80, 1, 2, 
	2, 81, 81, 81, 81, 81, 1, 2, 
	2, 82, 82, 82, 82, 82, 1, 2, 
	2, 83, 83, 83, 83, 83, 1, 2, 
	2, 84, 84, 84, 84, 84, 1, 2, 
	2, 85, 85, 85, 85, 85, 1, 2, 
	2, 86, 86, 86, 86, 86, 1, 2, 
	2, 87, 87, 87, 87, 87, 1, 2, 
	2, 88, 88, 88, 88, 88, 1, 2, 
	2, 89, 89, 89, 89, 89, 1, 2, 
	2, 90, 90, 90, 90, 90, 1, 2, 
	2, 91, 91, 91, 91, 91, 1, 2, 
	2, 92, 92, 92, 92, 92, 1, 2, 
	2, 93, 93, 93, 93, 93, 1, 2, 
	2, 94, 94, 94, 94, 94, 1, 2, 
	2, 95, 95, 95, 95, 95, 1, 2, 
	2, 96, 96, 96, 96, 96, 1, 2, 
	2, 1, 1, 0
};

static const char _http_req_parser_trans_targs[] = {
	2, 0, 3, 45, 4, 5, 24, 25, 
	30, 33, 41, 6, 5, 22, 6, 7, 
	8, 9, 10, 11, 12, 13, 12, 14, 
	15, 16, 14, 17, 18, 19, 64, 19, 
	20, 21, 20, 16, 21, 16, 23, 25, 
	26, 27, 28, 29, 39, 6, 5, 30, 
	37, 33, 6, 30, 5, 31, 33, 32, 
	6, 34, 5, 35, 6, 34, 5, 35, 
	36, 6, 5, 30, 38, 33, 5, 30, 
	38, 33, 40, 29, 42, 43, 44, 46, 
	47, 48, 49, 50, 51, 52, 53, 54, 
	55, 56, 57, 58, 59, 60, 61, 62, 
	63
};

static const char _http_req_parser_trans_actions[] = {
	1, 0, 3, 0, 62, 98, 17, 59, 
	59, 17, 59, 19, 15, 15, 0, 0, 
	0, 0, 0, 0, 65, 0, 21, 68, 
	0, 0, 23, 0, 0, 25, 33, 0, 
	27, 29, 29, 71, 0, 31, 15, 0, 
	5, 0, 0, 1, 1, 41, 82, 38, 
	7, 7, 53, 0, 90, 0, 13, 0, 
	78, 1, 74, 1, 56, 0, 94, 0, 
	0, 50, 86, 44, 47, 9, 35, 1, 
	11, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0
};

static const int http_req_parser_start = 1;
static const int http_req_parser_first_final = 64;
static const int http_req_parser_error = 0;

static const int http_req_parser_en_main = 1;


#line 97 "./c_src/request.rl"

// void
// add_field(http_req_parser_t* parser, char* name, const char* ptr)
// {
//     PyObject* val = mark_obj(parser->mark, ptr);
//     assert(val != NULL && "FIXME: mark_obj returned null");
//     if(PyDict_SetItemString(parser->fields, name, val) < 0)
//     {
//         assert(0 && "FIXME: Failed to set field value");
//     }
//     Py_XDECREF(val);
// }

// void
// add_uri(http_req_parser_t* parser, const char* ptr)
// {
//     PyObject* val = mark_obj(parser->mark_uri, ptr);
//     assert(val != NULL && "FIXME: mark_obj returned null");
//     if(!PyDict_SetItemString(parser->fields, "uri", val) < 0)
//     {
//         assert(0 && "FIXME: Failed to set field value");
//     }
//     Py_XDECREF(val);
// }

// void
// set_header_name(http_req_parser_t* parser, const char* ptr)
// {
//     parser->hdr_name = mark_obj(parser->mark, ptr);
//     assert(parser->hdr_name != NULL && "FIXME: mark_obj returned null");
// }

// void
// add_header(http_req_parser_t* parser, const char* ptr)
// {
//     PyObject* tuple = PyTuple_New(2);
//     PyObject* val = mark_obj(parser->mark, ptr);
// 
//     assert(tuple != NULL && "FIXME: PyTuple_New return null");
//     assert(val != NULL && "FIXME: mark_obj returned null");
//     assert(parser->hdr_name != NULL && "cant set header without a name");
//     assert(parser->headers != NULL && "cant set header without header list");
// 
//     PyTuple_SET_ITEM(tuple, 0, parser->hdr_name);
//     PyTuple_SET_ITEM(tuple, 1, val);
//     
//     if(!PyList_Append(parser->headers, tuple)) goto error;
//     
//     parser->hdr_name = NULL;
//     goto success;
// 
// error:
//     Py_XDECREF(tuple);
// success:
//     return;
// }

// void
// add_version(http_req_parser_t* parser)
// {
//     PyObject* major = NULL;
//     PyObject* minor = NULL;
//     PyObject* tuple = NULL;
//     
//     major = PyInt_FromLong(parser->vsn_major);
//     if(major == NULL) goto error;
//     
//     minor = PyInt_FromLong(parser->vsn_minor);
//     if(minor == NULL) goto error;
//     
//     tuple = PyTuple_New(2);
//     if(tuple == NULL) goto error;
//     
//     PyTuple_SET_ITEM(tuple, 0, major);
//     PyTuple_SET_ITEM(tuple, 1, minor);
//     
//     // SET_ITEM steals
//     major = NULL;
//     minor = NULL;
//     
//     if(PyDict_SetItemString(parser->fields, "version", tuple) < 0)
//     {
//         goto error;
//     }
//     
//     return;
//     
// error:
//     Py_XDECREF(major);
//     Py_XDECREF(minor);
//     Py_XDECREF(tuple);
// }

// void
// add_headers(http_req_parser_t* parser)
// {
//     assert(parser->fields != NULL && "parser fields disappeared");
//     assert(parser->headers != NULL && "parser headers disappeared");
// 
//     if(PyDict_SetItemString(parser->fields, "headers", parser->headers) < 0)
//     {
//         return;
//     }
//     
//     Py_DECREF(parser->headers);
//     parser->headers = NULL;
// }

int
exec_req_parser(RequestParser* parser)
{
    Request* request = parser->request;
    const char* p;
    const char* pe;
    int cs = parser->cs;
    
    p = parser->buffer;
    pe = parser->buffer + parser->buflen;
    
    reinit_buffer(parser->genbuf, p);
    reinit_buffer(parser->uribuf, p);
    
    
#line 364 "./c_src/request.c"
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
#line 13 "./c_src/request.rl"
	{
        assert(parser->genbuf->pos == NULL && "won't overwrite a mark.");
        parser->genbuf->pos = p;
    }
	break;
	case 1:
#line 18 "./c_src/request.rl"
	{
    }
	break;
	case 2:
#line 21 "./c_src/request.rl"
	{
    }
	break;
	case 3:
#line 24 "./c_src/request.rl"
	{
    }
	break;
	case 4:
#line 27 "./c_src/request.rl"
	{
        request->port = 0;
    }
	break;
	case 5:
#line 31 "./c_src/request.rl"
	{
        request->port = request->port*10 + ((*p)-'0');
    }
	break;
	case 6:
#line 35 "./c_src/request.rl"
	{
    }
	break;
	case 7:
#line 38 "./c_src/request.rl"
	{
    }
	break;
	case 8:
#line 41 "./c_src/request.rl"
	{
    }
	break;
	case 9:
#line 44 "./c_src/request.rl"
	{
        assert(parser->uribuf->pos != NULL && "wont overwrite uri mark");
        parser->uribuf->pos = p;
    }
	break;
	case 10:
#line 49 "./c_src/request.rl"
	{
    }
	break;
	case 11:
#line 52 "./c_src/request.rl"
	{
        request->vsn_major = 0;
    }
	break;
	case 12:
#line 56 "./c_src/request.rl"
	{
        request->vsn_major = request->vsn_major*10 + ((*p)-'0');
    }
	break;
	case 13:
#line 60 "./c_src/request.rl"
	{
        request->vsn_minor = 0;
    }
	break;
	case 14:
#line 64 "./c_src/request.rl"
	{
        request->vsn_minor = request->vsn_minor * 10 + ((*p)-'0');
    }
	break;
	case 15:
#line 68 "./c_src/request.rl"
	{
        assert(request->hdr_field == NULL && "header name already marked");        
        assert(parser->genbuf->pos == NULL && "wont overwrite a mark");
        parser->genbuf->pos = p;
    }
	break;
	case 16:
#line 74 "./c_src/request.rl"
	{
    }
	break;
	case 17:
#line 77 "./c_src/request.rl"
	{
        assert(request->hdr_name != NULL && "value must have a name");
        assert(parser->genbuf->pos == NULL && "wont overwrite a mark");
        parser->genbuf->pos = p;
    }
	break;
	case 18:
#line 83 "./c_src/request.rl"
	{
    }
	break;
	case 19:
#line 86 "./c_src/request.rl"
	{
        parser->body = p;
        {p++; goto _out; }
    }
	break;
#line 556 "./c_src/request.c"
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

#line 220 "./c_src/request.rl"

    parser->cs = cs;
    parser->nread += p - parser->buffer;
    
    assert(p < pe && "parser boundary error");

    if(parser->body != NULL)
    {
        assert(parser->mark->pos == NULL && "finished parsing with a mark");
        assert(parser->mark_uri->pos == NULL && "finished parsing with a uri");
        assert(parser->hdr_name == NULL && "finished parsing with a hdr name");
    }
    else if(parser->body == NULL && parser->cs == http_req_parser_error)
    {
        return 0;
    }

    fprintf(stderr, "STATE: %d\n", parser->cs);

    if(parser->genbuf->pos != NULL) save_buffer(parser->genbuf, p);
    if(parser->uribuf->pos != NULL) save_buffer(parser->uribuf, p);
    
    return 1;
}

//
//  Request Class
//

static PyObject*
Request_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    Request* self = NULL;
    //PyObject* parser = NULL;

    self = (Request*) type->tp_alloc(type, 0);
    if(self == NULL) goto error;

    self->parser = NULL;
    self->method = NULL;
    self->uri = NULL;
    self->scheme = NULL;
    self->host = NULL;
    self->port = 80;
    self->path = NULL;
    self->query = NULL;
    self->fragment = NULL;
    self->version = NULL;
    self->vsn_major = 0;
    self->vsn_minor = 0;
    self->headers = NULL;
    self->hdr_name = NULL;
    self->body = NULL;

    goto success;

error:
    Py_XDECREF(self);
    self = NULL;

success:
    return (PyObject*) self;
}

static void
Request_dealloc(Request* self)
{
    Py_XDECREF(self->parser);
    Py_XDECREF(self->method);
    Py_XDECREF(self->uri);
    Py_XDECREF(self->scheme);
    Py_XDECREF(self->host);
    Py_XDECREF(self->path);
    Py_XDECREF(self->query);
    Py_XDECREF(self->fragment);
    Py_XDECREF(self->version);
    Py_XDECREF(self->headers);
    Py_XDECREF(self->hdr_name);
    Py_XDECREF(self->body);
}

static PyMemberDef
Request_members[] = {
    {"method", T_OBJECT, offsetof(Request, method), READONLY,
        "This request's method as a string."},
    {"uri", T_OBJECT, offsetof(Request, uri), READONLY,
        "This request's original URI."},
    {"scheme", T_OBJECT, offsetof(Request, scheme), READONLY,
        "This request's URI scheme as a string."},
    {"host", T_OBJECT, offsetof(Request, host), READONLY,
        "This request's URI host as a string."},
    {"port", T_OBJECT, offsetof(Request, port), READONLY,
        "This request's URI port as a string."},
    {"path", T_OBJECT, offsetof(Request, path), READONLY,
        "This request's URI path as a string."},
    {"query", T_OBJECT, offsetof(Request, query), READONLY,
        "This request's URI query string as a string."},
    {"fragment", T_OBJECT, offsetof(Request, fragment), READONLY,
        "This request's URI fragment as a string."},
    {"version", T_OBJECT, offsetof(Request, version), READONLY,
        "This request's version as a two-tuple."},
    {"headers", T_OBJECT, offsetof(Request, headers), READONLY,
        "This request's headers as a list of two-tuples."},
    {NULL}
};

static PyMethodDef
Request_methods[] = {
    {NULL}
};

PyTypeObject RequestType = {
    PyObject_HEAD_INIT(NULL)
    0,                                          /*ob_size*/
    "pyhttpc.native.Request",                   /*tp_name*/
    sizeof(Request),                            /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)Request_dealloc,                /*tp_dealloc*/
    0,                                          /*tp_print*/
    0,                                          /*tp_getattr*/
    0,                                          /*tp_setattr*/
    0,                                          /*tp_compare*/
    0,                                          /*tp_repr*/
    0,                                          /*tp_as_number*/
    0,                                          /*tp_as_sequence*/
    0,                                          /*tp_as_mapping*/
    0,                                          /*tp_hash*/
    0,                                          /*tp_call*/
    0,                                          /*tp_str*/
    0,                                          /*tp_getattro*/
    0,                                          /*tp_setattro*/
    0,                                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,                         /*tp_flags*/
    "PyHttpC Request",                          /*tp_doc*/
    0,		                                    /*tp_traverse*/
    0,		                                    /*tp_clear*/
    0,		                                    /*tp_richcompare*/
    0,		                                    /*tp_weaklistoffset*/
    0,		                                    /*tp_iter*/
    0,		                                    /*tp_iternext*/
    Request_methods,                            /*tp_methods*/
    Request_members,                            /*tp_members*/
    0,                                          /*tp_getset*/
    0,                                          /*tp_base*/
    0,                                          /*tp_dict*/
    0,                                          /*tp_descr_get*/
    0,                                          /*tp_descr_set*/
    0,                                          /*tp_dictoffset*/
    0,                                          /*tp_init*/
    0,                                          /*tp_alloc*/
    Request_new,                                /*tp_new*/
};

//
// RequestParser Class
//

static PyObject*
RequestParser_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    RequestParser* self = NULL;
    PyObject* src = NULL;
    int cs = 0;
    
    self = (RequestParser*) type->tp_alloc(type, 0);
    if(self == NULL) goto error;

    if(!PyArg_ParseTuple(args, "O", &src)) goto error;
    if(!PyIter_Check(src))
    {
        PyErr_SetString(PyExc_TypeError, "Data source must be an iterator.");
        goto error;
    }
    self->source = PyObject_GetIter(src);
    if(self->source == NULL)
    {
        if(!PyErr_Occurred())
        {
            PyErr_SetString(PyExc_TypeError, "Unable create source iterator.");
        }
        goto error;
    }

    self->cs = 0;
    self->current = NULL;
    self->bufpos = NULL;
    self->buffer = NULL;
    self->buflen = 0;
    self->nread = 0;
    self->request = NULL;
    self->genbuf = init_buffer();
    if(self->genbuf == NULL)
    {
        PyErr_NoMemory();
        goto error;
    }
    self->uribuf = init_buffer();
    if(self->uribuf == NULL)
    {
        PyErr_NoMemory();
        goto error;
    }

    
#line 774 "./c_src/request.c"
	{
	cs = http_req_parser_start;
	}

#line 424 "./c_src/request.rl"
    self->cs = cs;

    goto success;

error:
    Py_XDECREF(self);
    self = NULL;

success:
    return (PyObject*) self;
}

static void
RequestParser_dealloc(RequestParser* self)
{
    Py_XDECREF(self->source);
    Py_XDECREF(self->current);
    Py_XDECREF(self->request);
    free_buffer(self->genbuf);
    free_buffer(self->uribuf);
}

PyObject*
RequestParser_GET_ITER(RequestParser* self)
{
    Py_INCREF(self);
    return (PyObject*) self;
}

PyObject*
RequestParser_ITER_NEXT(RequestParser* self)
{
    PyObject* tpl = NULL;
    PyObject* req = NULL;
    
    tpl = Py_BuildValue("(O)", self);
    if(tpl == NULL) return NULL;

    req = PyObject_CallObject((PyObject*) &RequestType, tpl);

    Py_XDECREF(tpl);
    return req;
}

static PyMemberDef
RequestParser_members[] = {
    {NULL}
};

static PyMethodDef
RequestParser_methods[] = {
    {NULL}
};

PyTypeObject RequestParserType = {
    PyObject_HEAD_INIT(NULL)
    0,                                          /*ob_size*/
    "pyhttpc.native.RequestParser",             /*tp_name*/
    sizeof(RequestParser),                      /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)RequestParser_dealloc,          /*tp_dealloc*/
    0,                                          /*tp_print*/
    0,                                          /*tp_getattr*/
    0,                                          /*tp_setattr*/
    0,                                          /*tp_compare*/
    0,                                          /*tp_repr*/
    0,                                          /*tp_as_number*/
    0,                                          /*tp_as_sequence*/
    0,                                          /*tp_as_mapping*/
    0,                                          /*tp_hash*/
    0,                                          /*tp_call*/
    0,                                          /*tp_str*/
    0,                                          /*tp_getattro*/
    0,                                          /*tp_setattro*/
    0,                                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_ITER,  /*tp_flags*/
    "PyHttpC Request Parser",                   /*tp_doc*/
    0,		                                    /*tp_traverse*/
    0,		                                    /*tp_clear*/
    0,		                                    /*tp_richcompare*/
    0,		                                    /*tp_weaklistoffset*/
    (getiterfunc)RequestParser_GET_ITER,		/*tp_iter*/
    (iternextfunc)RequestParser_ITER_NEXT,		/*tp_iternext*/
    RequestParser_methods,                      /*tp_methods*/
    RequestParser_members,                      /*tp_members*/
    0,                                          /*tp_getset*/
    0,                                          /*tp_base*/
    0,                                          /*tp_dict*/
    0,                                          /*tp_descr_get*/
    0,                                          /*tp_descr_set*/
    0,                                          /*tp_dictoffset*/
    0,                                          /*tp_init*/
    0,                                          /*tp_alloc*/
    RequestParser_new,                          /*tp_new*/
};

