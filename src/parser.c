
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "parser.h"

enum STATE {
    ST_DONE=1,
    
    ST_REQ_INIT,
    ST_REQ_METHOD,
    ST_REQ_BEFORE_URL,
    ST_REQ_SCHEME,
    ST_REQ_SCHEME_SLASH,
    ST_REQ_SCHEME_SLASH_SLASH,
    ST_REQ_HOST_START,
    ST_REQ_HOST,
    ST_REQ_PORT_START,
    ST_REQ_PORT,
    ST_REQ_PATH,
    ST_REQ_QS_START,
    ST_REQ_QS,
    ST_REQ_FRAGMENT_START,
    ST_REQ_FRAGMENT,
    ST_REQ_URI_DONE,
    ST_REQ_HTTP_START,
    ST_REQ_HTTP_H,
    ST_REQ_HTTP_HT,
    ST_REQ_HTTP_HTT,
    ST_REQ_HTTP_HTTP,
    ST_REQ_HTTP_MAJOR_START,
    ST_REQ_HTTP_MAJOR,
    ST_REQ_HTTP_MINOR_START,
    ST_REQ_HTTP_MINOR,
    ST_REQ_LINE_ALMOST_DONE,

    ST_RES_INIT, // 28
    ST_RES_H,
    ST_RES_HT,
    ST_RES_HTT,
    ST_RES_HTTP,
    ST_RES_HTTP_MAJOR_START,
    ST_RES_HTTP_MAJOR,
    ST_RES_HTTP_MINOR_START,
    ST_RES_HTTP_MINOR,
    ST_RES_STATUS_START,
    ST_RES_STATUS,
    ST_RES_STATUS_ALMOST_DONE,

    ST_HDR_FIELD_START, // 40
    ST_HDR_FIELD,
    ST_HDR_GENERAL,
    ST_HDR_C,
    ST_HDR_CO,
    ST_HDR_CON,
    ST_HDR_IN_CONNECTION,
    ST_HDR_CONNECTION,
    ST_HDR_IN_CONTENT_LENGTH,
    ST_HDR_CONTENT_LENGTH,
    ST_HDR_IN_TRANSFER_ENCODING,
    ST_HDR_TRANSFER_ENCODING,

    ST_HDR_BEFORE_VALUE, // 52
    ST_HDR_VALUE_START,
    ST_HDR_VALUE,
    ST_HDR_VALUE_ALMOST_DONE,
    ST_HDR_VALUE_MAYBE_DONE,
    ST_HDR_IN_CONTENT_LENGTH_VALUE,
    ST_HDR_IN_KEEP_ALIVE,
    ST_HDR_IN_CLOSE,
    ST_HDR_IN_CHUNKED,

    ST_HDR_ALMOST_DONE, // 61
    ST_HDR_DONE,

    ST_CHUNK_SIZE_START, //63
    ST_CHUNK_SIZE,
    ST_CHUNK_SIZE_PARAMETERS,
    ST_CHUNK_SIZE_ALMOST_DONE,
    ST_CHUNK_DATA,
    ST_CHUNK_DATA_ALMOST_DONE,
    ST_CHUNK_DATA_DONE,

    ST_BODY_IDENTITY,  // 70
    ST_BODY_IDENTITY_EOF
};

#define FLAG_CONNECTION_CLOSE 1
#define FLAG_CONNECTION_KEEP_ALIVE 2
#define FLAG_CHUNKED 4

static const char* CONTENT_LENGTH = "content-length";
static const size_t CONTENT_LENGTH_LENGTH = 14;

static const char* CONNECTION = "connection";
static const size_t CONNECTION_LENGTH = 10;
static const char* CONNECTION_KEEP_ALIVE = "keep-alive";
static const size_t CONNECTION_KEEP_ALIVE_LENGTH = 10;
static const char* CONNECTION_CLOSE = "close";
static const size_t CONNECTION_CLOSE_LENGTH = 5;

static const char* TRANSFER_ENCODING = "transfer-encoding";
static const size_t TRANSFER_ENCODING_LENGTH = 17;
static const char* TRANSFER_ENCODING_CHUNKED = "chunked";
static const size_t TRANSFER_ENCODING_CHUNKED_LENGTH = 7;

static const int HEXVAL[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,

    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

/*
    Given a character C:

    C>>5 is a number 0-7 which selects which integer its in.
    1<<(ch&0x1f) selects the bit to test in that integer.

    So, table[C>>5] & (1<<(ch&0x1f)) gives a 1 or 0 that we
    use in conditionals.

    Generally it should just be enough to use the defined
    macro.
*/
static const uint32_t  allowed_in_header_field[] = {
    0x00000000, // 0000 0000 0000 0000  0000 0000 0000 0000

                // ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"! 
    0x03ff2000, // 0000 0011 1111 1111  0010 0000 0000 0000

                // _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@
    0x07fffffe, // 0000 0111 1111 1111  1111 1111 1111 1110

                //  ~}| {zyx wvut srqp  onml kjih gfed cba`
    0x07fffffe, // 0000 0111 1111 1111  1111 1111 1111 1110

    0x00000000, // 0000 0000 0000 0000  0000 0000 0000 0000
    0x00000000, // 0000 0000 0000 0000  0000 0000 0000 0000
    0x00000000, // 0000 0000 0000 0000  0000 0000 0000 0000
    0x00000000  // 0000 0000 0000 0000  0000 0000 0000 0000
};
#define OK_IN_HEADER_FIELD(c) (allowed_in_header_field[c>>5] & (1<<(ch&0x1f)))

static const uint32_t  allowed_after_host[] = {
    0xffffdbfe, /* 1111 1111 1111 1111  1101 1011 1111 1110 */

                /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
    0x7ffffff6, /* 0111 1111 1111 1111  1111 1111 1111 0110 */

                /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
};
#define OK_AFTER_HOST(c) (allowed_after_host[c>>5] & (1<<(ch&0x1f)))

#define CR '\r'
#define LF '\n'
#define LOWER(c) (unsigned char)(c | 0x20)
#define IS_LOWER(c) (c >= 'a' && c <= 'z')
#define IS_DIGIT(c) (c >= '0' && c <= '9')
#define IS_SPACE(c) (c == ' ' || c == '\t')

#ifndef MIN
# define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

// Flow Control
#define INIT_STATE(action)              \
do {                                    \
    p->stateact = action;               \
    p->stateptr = p->buf;               \
} while(0);

#define SUSPEND(act)                    \
if(p->action)                           \
{                                       \
    assert(act == p->stateact);         \
    p->action = p->stateact;            \
    p->data = p->stateptr;              \
    p->length = p->buf - p->data;       \
    p->stateact = HTTP_CONTINUE;        \
    p->stateptr = NULL;                 \
}

#define JUMP(st) \
    {p->state = st; break;}

#define SKIP_IF(cond) \
    if(cond) {break;}

#define NEXT_IF(cond, st)               \
    if(cond) {                          \
        p->state = st;                  \
        break;                          \
    }

#define SUSP_IF(cond, st, act)          \
    if(cond) {                          \
        SUSPEND(act);                   \
        p->state = st;                  \
        break;                          \
    }

#define INIT_IF(cond, st, act)          \
    if(cond) {                          \
        p->state = st;                  \
        p->stateact = act;              \
        p->stateptr = p->buf;           \
        break;                          \
    }

#define ERROR_IF(cond) \
    if(cond) {goto error;}

#define ERROR goto error;

http_parser*
http_init_parser(ushort type)
{
    http_parser* ret = malloc(sizeof(http_parser));
    if(ret == NULL) return NULL;

    ret->action = HTTP_CONTINUE;
    ret->data = NULL;
    ret->length = 0;
    ret->status = 0;
    ret->vmajor = 0;
    ret->vminor = 0;
    ret->content_length = -1;
    ret->content_read = -1;
    ret->is_request = type == HTTP_REQUEST_PARSER ? 1 : 0;
    ret->state = ST_REQ_INIT;
    ret->stateact = HTTP_CONTINUE;
    ret->stateptr = NULL;
    ret->buf = NULL;
    ret->bufend = NULL;
    ret->hdr_index = 0;
    ret->hdr_value = 0;
    ret->flags = 0;
    return ret;
}

size_t
http_get_buffer(http_parser* p, const uchar** buf)
{
    if(p->stateptr == NULL)
    {
        *buf = NULL;
        return 0;
    }
    
    if(p->stateptr >= p->bufend)
    {
        *buf = NULL;
        return 0;
    }

    *buf = p->stateptr;
    return (p->bufend - p->stateptr);
}

void
http_set_buffer(http_parser* p, const uchar* buf, size_t len)
{
    p->buf = buf;
    p->stateptr = buf;
    p->bufend = buf + len;
}

ushort
http_should_keep_alive(http_parser* p)
{
    if(p->vmajor > 0 && p->vminor > 0)
    {
        if(p->flags & FLAG_CONNECTION_CLOSE) return 0;
        return 1;
    }
    else
    {
        if(p->flags & FLAG_CONNECTION_KEEP_ALIVE) return 1;
        return 0;
    }
}

ushort
http_run_parser(http_parser* p)
{
    uchar c = 0;
    uchar ch;
    int hv;
    ssize_t to_read;
    
    if(p->buf == NULL) return HTTP_ERROR;

    p->action = HTTP_CONTINUE;
    p->data = NULL;
    p->length = 0;

    for(;p->buf != p->bufend && p->action == HTTP_CONTINUE; p->buf++)
    {
        ch = *(p->buf);
        
        //fprintf(stderr, "S&A: %2u %2u %c\n", p->state, p->stateact, ch);

        switch(p->state) {
            case ST_DONE:
                ERROR;

            case ST_REQ_INIT:
                INIT_STATE(HTTP_METHOD);
                c = LOWER(ch);
                NEXT_IF(IS_LOWER(c), ST_REQ_METHOD);
                ERROR;

            case ST_REQ_METHOD:
                c = LOWER(ch);
                SKIP_IF(IS_LOWER(c));
                SUSPEND(HTTP_METHOD);
                NEXT_IF(IS_SPACE(ch), ST_REQ_BEFORE_URL);
                ERROR;

            case ST_REQ_BEFORE_URL:
                SKIP_IF(IS_SPACE(ch));
                INIT_IF(ch == '/', ST_REQ_PATH, HTTP_PATH);
                c = LOWER(c);
                INIT_IF(IS_LOWER(c), ST_REQ_SCHEME, HTTP_SCHEME);
                ERROR;

            case ST_REQ_SCHEME:
                c = LOWER(ch);
                SKIP_IF(IS_LOWER(c));
                NEXT_IF(ch == ':', ST_REQ_SCHEME_SLASH);
                ERROR;
            
            case ST_REQ_SCHEME_SLASH:
                SUSPEND(HTTP_SCHEME);
                NEXT_IF(ch == '/', ST_REQ_SCHEME_SLASH_SLASH);
                ERROR;

            case ST_REQ_SCHEME_SLASH_SLASH:
                NEXT_IF(ch == '/', ST_REQ_HOST_START);
                ERROR;
            
            case ST_REQ_HOST_START:
                INIT_STATE(HTTP_HOST);
                c = LOWER(ch);
                NEXT_IF(IS_LOWER(c), ST_REQ_HOST);
                ERROR;

            case ST_REQ_HOST:
                c = LOWER(ch);
                SKIP_IF(IS_LOWER(c));
                SKIP_IF(IS_DIGIT(ch) || ch == '.' || ch == '-');
                SUSPEND(HTTP_HOST);
                NEXT_IF(ch == ':', ST_REQ_PORT_START);
                INIT_IF(ch == '/', ST_REQ_PATH, HTTP_PATH);
                NEXT_IF(ch == ' ', ST_REQ_HTTP_START);
                ERROR;

            case ST_REQ_PORT_START:
                INIT_STATE(HTTP_PORT);
                ERROR_IF(!IS_DIGIT(ch));
                JUMP(ST_REQ_PORT);

            case ST_REQ_PORT:
                SKIP_IF(IS_DIGIT(ch));
                SUSPEND(HTTP_PORT);
                INIT_IF(ch == '/', ST_REQ_PATH, HTTP_PATH);
                NEXT_IF(ch == ' ', ST_REQ_HTTP_START);
                ERROR;

            case ST_REQ_PATH:
                SKIP_IF(OK_AFTER_HOST(ch));
                SUSPEND(HTTP_PATH);
                NEXT_IF(ch == ' ', ST_REQ_HTTP_START);
                NEXT_IF(ch == '?', ST_REQ_QS_START);
                NEXT_IF(ch == '#', ST_REQ_FRAGMENT_START);
                if(ch == CR)
                {
                    p->vminor = 9;
                    JUMP(ST_REQ_LINE_ALMOST_DONE);
                }
                if(ch == LF)
                {
                    p->vminor = 9;
                    JUMP(ST_HDR_FIELD_START);
                }
                ERROR;

            case ST_REQ_QS_START:
                INIT_STATE(HTTP_QS);
                NEXT_IF(OK_AFTER_HOST(ch), ST_REQ_QS);
                SUSPEND(HTTP_QS);
                NEXT_IF(ch == ' ', ST_REQ_HTTP_START);
                INIT_IF(ch == '#', ST_REQ_FRAGMENT_START, HTTP_FRAGMENT);
                if(ch == CR)
                {
                    p->vminor = 9;
                    JUMP(ST_REQ_LINE_ALMOST_DONE);
                }
                if(ch == LF)
                {
                    p->vminor = 9;
                    JUMP(ST_HDR_FIELD_START);
                }
                ERROR;

            case ST_REQ_QS:
                SKIP_IF(OK_AFTER_HOST(ch));
                SUSPEND(HTTP_QS);
                NEXT_IF(ch == ' ', ST_REQ_HTTP_START);
                INIT_IF(ch == '#', ST_REQ_FRAGMENT_START, HTTP_FRAGMENT);
                if(ch == CR)
                {
                    p->vminor = 9;
                    JUMP(ST_REQ_LINE_ALMOST_DONE);
                }
                if(ch == LF)
                {
                    p->vminor = 9;
                    JUMP(ST_HDR_FIELD_START);
                }
                ERROR;

            case ST_REQ_FRAGMENT_START:
                INIT_STATE(HTTP_FRAGMENT);
                NEXT_IF(OK_AFTER_HOST(ch), ST_REQ_FRAGMENT);
                SUSPEND(HTTP_FRAGMENT);
                NEXT_IF(ch == ' ', ST_REQ_HTTP_START);
                if(ch == CR)
                {
                    p->vminor = 9;
                    JUMP(ST_REQ_LINE_ALMOST_DONE);
                }
                if(ch == LF)
                {
                    p->vminor = 9;
                    JUMP(ST_HDR_FIELD_START);
                }
                ERROR;

            case ST_REQ_FRAGMENT:
                SKIP_IF(OK_AFTER_HOST(ch));
                SUSPEND(HTTP_FRAGMENT);
                NEXT_IF(ch == ' ', ST_REQ_HTTP_START);
                if(ch == CR)
                {
                    p->vminor = 9;
                    JUMP(ST_REQ_LINE_ALMOST_DONE);
                }
                if(ch == LF)
                {
                    p->vminor = 9;
                    JUMP(ST_HDR_FIELD_START);
                }
                ERROR;

            case ST_REQ_HTTP_START:
                SKIP_IF(ch == ' ');
                NEXT_IF(ch == 'H', ST_REQ_HTTP_H);
                ERROR;

            case ST_REQ_HTTP_H:
                NEXT_IF(ch == 'T', ST_REQ_HTTP_HT);
                ERROR;

            case ST_REQ_HTTP_HT:
                NEXT_IF(ch == 'T', ST_REQ_HTTP_HTT);
                ERROR;

            case ST_REQ_HTTP_HTT:
                NEXT_IF(ch == 'P', ST_REQ_HTTP_HTTP);
                ERROR;

            case ST_REQ_HTTP_HTTP:
                NEXT_IF(ch == '/', ST_REQ_HTTP_MAJOR_START);
                ERROR;

            case ST_REQ_HTTP_MAJOR_START:
                ERROR_IF(!IS_DIGIT(ch));
                p->vmajor = ch - '0';
                p->state = ST_REQ_HTTP_MAJOR;
                break;

            case ST_REQ_HTTP_MAJOR:
                NEXT_IF(ch == '.', ST_REQ_HTTP_MINOR_START)
                ERROR_IF(!IS_DIGIT(ch));
                p->vmajor *= 10;
                p->vmajor += ch - '0';
                ERROR_IF(p->vmajor > 999);
                break;

            case ST_REQ_HTTP_MINOR_START:
                ERROR_IF(!IS_DIGIT(ch));
                p->vminor = ch - '0';
                p->state = ST_REQ_HTTP_MINOR;
                break;

            case ST_REQ_HTTP_MINOR:
                NEXT_IF(ch == CR, ST_REQ_LINE_ALMOST_DONE);
                NEXT_IF(ch == LF, ST_HDR_FIELD_START);
                ERROR_IF(!IS_DIGIT(ch));
                p->vminor *= 10;
                p->vminor += ch - '0';
                ERROR_IF(p->vminor > 999);
                break;

            case ST_REQ_LINE_ALMOST_DONE:
                ERROR_IF(ch != LF);
                JUMP(ST_HDR_FIELD_START);
                
            case ST_HDR_FIELD_START:
                NEXT_IF(ch == CR, ST_HDR_ALMOST_DONE);
                NEXT_IF(ch == LF, ST_HDR_DONE);
                INIT_STATE(HTTP_HEADER_FIELD);
                p->hdr_index = 0;
                p->hdr_state = 0;
                c = LOWER(ch);
                ERROR_IF(!IS_LOWER(c));
                NEXT_IF(c == 'c', ST_HDR_C);
                NEXT_IF(c == 't', ST_HDR_IN_TRANSFER_ENCODING);
                JUMP(ST_HDR_FIELD);

            case ST_HDR_FIELD:
                p->hdr_index = 0;
                p->hdr_state = 0;
                SKIP_IF(OK_IN_HEADER_FIELD(ch));
                SUSPEND(HTTP_HEADER_FIELD);
                NEXT_IF(IS_SPACE(ch), ST_HDR_BEFORE_VALUE);
                NEXT_IF(ch == ':', ST_HDR_VALUE_START);
                ERROR;
            
            case ST_HDR_C:
                c = LOWER(ch);
                NEXT_IF(c == 'o', ST_HDR_CO);
                NEXT_IF(OK_IN_HEADER_FIELD(ch), ST_HDR_FIELD);
                SUSPEND(HTTP_HEADER_FIELD);
                NEXT_IF(IS_SPACE(ch), ST_HDR_BEFORE_VALUE);
                NEXT_IF(ch == ':', ST_HDR_VALUE_START);
                ERROR;

            case ST_HDR_CO:
                c = LOWER(ch);
                NEXT_IF(c == 'n', ST_HDR_CON);
                NEXT_IF(OK_IN_HEADER_FIELD(ch), ST_HDR_FIELD);
                SUSPEND(HTTP_HEADER_FIELD);
                NEXT_IF(IS_SPACE(ch), ST_HDR_BEFORE_VALUE);
                NEXT_IF(ch == ':', ST_HDR_VALUE_START);
                ERROR;
            
            case ST_HDR_CON:
                c = LOWER(ch);
                p->hdr_index = 4;
                NEXT_IF(c == 'n', ST_HDR_IN_CONNECTION);
                NEXT_IF(c == 't', ST_HDR_IN_CONTENT_LENGTH);
                NEXT_IF(OK_IN_HEADER_FIELD(ch), ST_HDR_FIELD);
                SUSPEND(HTTP_HEADER_FIELD);
                NEXT_IF(IS_SPACE(ch), ST_HDR_BEFORE_VALUE);
                NEXT_IF(ch == ':', ST_HDR_VALUE_START);
                ERROR;
            
            case ST_HDR_IN_CONNECTION:
                SUSP_IF(IS_SPACE(ch), ST_HDR_BEFORE_VALUE, HTTP_HEADER_FIELD);
                SUSP_IF(ch == ':', ST_HDR_VALUE_START, HTTP_HEADER_FIELD);
                ERROR_IF(!OK_IN_HEADER_FIELD(ch));
                if(p->hdr_index >= CONNECTION_LENGTH)
                {
                    p->hdr_index = 0;
                    p->hdr_state = 0;
                    JUMP(ST_HDR_FIELD);
                }
                c = LOWER(ch);
                NEXT_IF(c != CONNECTION[p->hdr_index], ST_HDR_FIELD);
                p->hdr_index += 1;
                if(p->hdr_index >= CONNECTION_LENGTH)
                {
                    p->hdr_state = ST_HDR_CONNECTION;
                }
                break;

            case ST_HDR_IN_CONTENT_LENGTH:
                SUSP_IF(IS_SPACE(ch), ST_HDR_BEFORE_VALUE, HTTP_HEADER_FIELD);
                SUSP_IF(ch == ':', ST_HDR_VALUE_START, HTTP_HEADER_FIELD);
                ERROR_IF(!OK_IN_HEADER_FIELD(ch));
                if(p->hdr_index >= CONTENT_LENGTH_LENGTH)
                {
                    p->hdr_index = 0;
                    p->hdr_state = 0;
                    JUMP(ST_HDR_FIELD);
                }
                c = LOWER(ch);
                NEXT_IF(c != CONTENT_LENGTH[p->hdr_index], ST_HDR_FIELD);
                p->hdr_index += 1;
                if(p->hdr_index >= CONTENT_LENGTH_LENGTH)
                {
                    p->hdr_state = ST_HDR_CONTENT_LENGTH;
                }
                break;
            
            case ST_HDR_IN_TRANSFER_ENCODING:
                SUSP_IF(IS_SPACE(ch), ST_HDR_BEFORE_VALUE, HTTP_HEADER_FIELD);
                SUSP_IF(ch == ':', ST_HDR_VALUE_START, HTTP_HEADER_FIELD);
                ERROR_IF(!OK_IN_HEADER_FIELD(ch));
                // Already matched T
                if(p->hdr_index == 0) p->hdr_index += 1;
                if(p->hdr_index > TRANSFER_ENCODING_LENGTH)
                {
                    p->hdr_index = 0;
                    p->hdr_state = 0;
                    JUMP(ST_HDR_FIELD);
                }
                c = LOWER(ch);
                NEXT_IF(c != TRANSFER_ENCODING[p->hdr_index], ST_HDR_FIELD);
                p->hdr_index += 1;
                if(p->hdr_index >= TRANSFER_ENCODING_LENGTH)
                {
                    p->hdr_state = ST_HDR_TRANSFER_ENCODING;
                }
                break;

            case ST_HDR_BEFORE_VALUE:
                SKIP_IF(ch == ' ');
                NEXT_IF(ch == ':', ST_HDR_VALUE_START);
                ERROR;

            case ST_HDR_VALUE_START:
                SKIP_IF(IS_SPACE(ch));

                INIT_STATE(HTTP_HEADER_VALUE);
                NEXT_IF(ch == CR, ST_HDR_VALUE_ALMOST_DONE);
                NEXT_IF(ch == LF, ST_HDR_VALUE_MAYBE_DONE);
                
                p->hdr_index = 1;
                
                c = LOWER(ch);
                if(p->hdr_state == ST_HDR_CONTENT_LENGTH)
                {
                    NEXT_IF(!IS_DIGIT(ch), ST_HDR_VALUE);
                    p->content_length = ch - '0';
                    JUMP(ST_HDR_IN_CONTENT_LENGTH_VALUE);
                }
                else if(p->hdr_state == ST_HDR_CONNECTION)
                {
                    NEXT_IF(c == 'c', ST_HDR_IN_CLOSE);
                    NEXT_IF(c == 'k', ST_HDR_IN_KEEP_ALIVE);
                }
                else if(p->hdr_state == ST_HDR_TRANSFER_ENCODING)
                {
                    NEXT_IF(c == 'c', ST_HDR_IN_CHUNKED);
                }
                JUMP(ST_HDR_VALUE);

            case ST_HDR_VALUE:
                p->hdr_index = 0;
                p->hdr_state = 0;
                NEXT_IF(ch == CR, ST_HDR_VALUE_ALMOST_DONE);
                NEXT_IF(ch == LF, ST_HDR_VALUE_MAYBE_DONE);
                break;

            case ST_HDR_IN_CONTENT_LENGTH_VALUE:
                p->hdr_state = ST_HDR_IN_CONTENT_LENGTH_VALUE;
                NEXT_IF(ch == CR, ST_HDR_VALUE_ALMOST_DONE);
                NEXT_IF(ch == LF, ST_HDR_VALUE_MAYBE_DONE);
                NEXT_IF(!IS_DIGIT(ch), ST_HDR_VALUE);
                p->content_length *= 10;
                p->content_length += ch - '0';
                break;
                
            case ST_HDR_IN_CLOSE:
                p->hdr_state = ST_HDR_IN_CLOSE;
                NEXT_IF(ch == CR, ST_HDR_VALUE_ALMOST_DONE);
                NEXT_IF(ch == LF, ST_HDR_VALUE_MAYBE_DONE);
                if(p->hdr_index < CONNECTION_CLOSE_LENGTH)
                {
                    NEXT_IF(LOWER(ch) != CONNECTION_CLOSE[p->hdr_index], ST_HDR_VALUE);
                    p->hdr_index += 1;
                    break;
                }
                SKIP_IF(IS_SPACE(ch));
                JUMP(ST_HDR_VALUE);
            
            case ST_HDR_IN_KEEP_ALIVE:
                p->hdr_state = ST_HDR_IN_KEEP_ALIVE;
                NEXT_IF(ch == CR, ST_HDR_VALUE_ALMOST_DONE);
                NEXT_IF(ch == LF, ST_HDR_VALUE_MAYBE_DONE);
                if(p->hdr_index < CONNECTION_KEEP_ALIVE_LENGTH)
                {
                    c = LOWER(ch);
                    NEXT_IF(c != CONNECTION_KEEP_ALIVE[p->hdr_index], ST_HDR_VALUE);
                    p->hdr_index += 1;
                    break;
                }
                SKIP_IF(IS_SPACE(ch));
                JUMP(ST_HDR_VALUE);
            
            case ST_HDR_IN_CHUNKED:
                p->hdr_state = ST_HDR_IN_CHUNKED;
                NEXT_IF(ch == CR, ST_HDR_VALUE_ALMOST_DONE);
                NEXT_IF(ch == LF, ST_HDR_VALUE_MAYBE_DONE);
                if(p->hdr_index < TRANSFER_ENCODING_CHUNKED_LENGTH)
                {
                    c = LOWER(ch);
                    NEXT_IF(c != TRANSFER_ENCODING_CHUNKED[p->hdr_index], ST_HDR_VALUE);
                    p->hdr_index += 1;
                    break;
                }
                SKIP_IF(IS_SPACE(ch));
                JUMP(ST_HDR_VALUE);

            case ST_HDR_VALUE_ALMOST_DONE:
                NEXT_IF(ch == LF, ST_HDR_VALUE_MAYBE_DONE);
                ERROR;
            
            case ST_HDR_VALUE_MAYBE_DONE:
                // Checking for a continuation line.
                if(IS_SPACE(ch)) // Not done
                {
                    if(p->hdr_state == ST_HDR_IN_CONTENT_LENGTH_VALUE)
                    {
                        p->content_length = -1;
                        p->hdr_index = 0;
                        p->hdr_state = 0;
                        JUMP(ST_HDR_VALUE);
                    }
                    else if(p->hdr_state)
                    {
                        JUMP(p->hdr_state);
                    }
                    else
                    {
                        JUMP(ST_HDR_VALUE);
                    }
                }
                // Set values for special headers.
                else if(p->hdr_state == ST_HDR_IN_CLOSE)
                {
                    if(p->flags & FLAG_CONNECTION_KEEP_ALIVE)
                    {
                        ERROR;
                    }
                    p->flags |= FLAG_CONNECTION_CLOSE;
                }
                else if(p->hdr_state == ST_HDR_IN_KEEP_ALIVE)
                {
                    if(p->flags & FLAG_CONNECTION_CLOSE)
                    {
                        ERROR;
                    }
                    p->flags |= FLAG_CONNECTION_KEEP_ALIVE;
                }
                else if(p->hdr_state == ST_HDR_IN_CHUNKED)
                {
                    p->flags |= FLAG_CHUNKED;
                }

                p->hdr_index = 0;
                p->hdr_state = 0;

                SUSP_IF(ch == CR, ST_HDR_ALMOST_DONE, HTTP_HEADER_VALUE);
                SUSP_IF(ch == LF, ST_HDR_DONE, HTTP_HEADER_VALUE);
                SUSPEND(HTTP_HEADER_VALUE);
                
                // Mark header, return value.
                INIT_STATE(HTTP_HEADER_FIELD);
                p->hdr_index = 0;
                p->hdr_state = 0;
                c = LOWER(ch);
                ERROR_IF(!IS_LOWER(c));
                NEXT_IF(c == 'c', ST_HDR_C);
                NEXT_IF(c == 't', ST_HDR_IN_TRANSFER_ENCODING);
                JUMP(ST_HDR_FIELD);
                ERROR;

            case ST_HDR_DONE:
                //ERROR;

            case ST_HDR_ALMOST_DONE:
                ERROR_IF(ch != LF);

                p->content_read = 0;
                if(p->flags & FLAG_CHUNKED)
                {
                    INIT_STATE(HTTP_HEADERS_DONE);
                    SUSPEND(HTTP_HEADERS_DONE);
                    p->content_length = 0;
                    JUMP(ST_CHUNK_SIZE_START);
                }
                else if(p->content_length == 0)
                {
                    INIT_STATE(HTTP_DONE);
                    SUSPEND(HTTP_DONE);
                    JUMP(ST_REQ_INIT);
                }
                else if(p->content_length >= 0)
                {
                    INIT_STATE(HTTP_HEADERS_DONE);
                    SUSPEND(HTTP_HEADERS_DONE);
                    p->content_read = 0;
                    JUMP(ST_BODY_IDENTITY);
                }
                else if(p->is_request && http_should_keep_alive(p))
                {
                    INIT_STATE(HTTP_DONE);
                    SUSPEND(HTTP_DONE);
                    JUMP(ST_REQ_INIT);
                }
                else
                {
                    INIT_STATE(HTTP_HEADERS_DONE);
                    SUSPEND(HTTP_HEADERS_DONE);
                    JUMP(ST_BODY_IDENTITY_EOF);
                }
            
            case ST_BODY_IDENTITY:
                to_read = (ssize_t) p->content_length - p->content_read;
                to_read = MIN(p->bufend - p->buf, to_read);
                if(p->content_read + to_read == p->content_length)
                {
                    INIT_STATE(HTTP_DONE);
                }
                else
                {
                    INIT_STATE(HTTP_BODY);
                }
                if(to_read > 0)
                {
                    p->content_read += to_read;
                    p->buf += to_read;
                    if(p->content_read == p->content_length)
                    {
                        SUSPEND(HTTP_DONE);
                        JUMP(ST_REQ_INIT);
                    }
                    else
                    {
                        SUSPEND(HTTP_BODY);
                    }
                }
                break;

            case ST_BODY_IDENTITY_EOF:
                fprintf(stderr, "ST_BODY_IDENTITY_EOF\n");
                to_read = p->bufend - p->buf;
                if(to_read > 0)
                {
                    INIT_STATE(HTTP_BODY);
                    p->buf += to_read;
                    SUSPEND(HTTP_BODY);
                    p->content_read += to_read;
                }
                else
                {
                    INIT_STATE(HTTP_DONE);
                    SUSPEND(HTTP_DONE);
                }
                break;
            
            case ST_CHUNK_SIZE_START:
                hv = HEXVAL[(int) ch];
                if(hv < 0) ERROR;
                p->content_length = hv;
                p->content_read = 0;
                JUMP(ST_CHUNK_SIZE);
            
            case ST_CHUNK_SIZE:
                NEXT_IF(ch == CR, ST_CHUNK_SIZE_ALMOST_DONE);
                NEXT_IF(ch == LF, ST_CHUNK_DATA);
                NEXT_IF(IS_SPACE(ch), ST_CHUNK_SIZE_PARAMETERS);
                NEXT_IF(ch == ';', ST_CHUNK_SIZE_PARAMETERS);
                hv = HEXVAL[(int) ch];
                ERROR_IF(hv < 0);
                p->content_length *= 16;
                p->content_length += hv;
                break;

            case ST_CHUNK_SIZE_PARAMETERS:
                // Just discard till end of line.
                SKIP_IF(ch != LF);
                if(p->content_length > 0)
                {
                    p->content_read = 0;
                    JUMP(ST_CHUNK_DATA);
                }
                else
                {
                    INIT_STATE(HTTP_DONE);
                    SUSPEND(HTTP_DONE);
                    JUMP(ST_REQ_INIT);
                }

            case ST_CHUNK_SIZE_ALMOST_DONE:
                if(p->content_length > 0 && ch == LF)
                {
                    JUMP(ST_CHUNK_DATA);
                }
                else if(ch == LF)
                {
                    INIT_STATE(HTTP_DONE);
                    SUSPEND(HTTP_DONE);
                    JUMP(ST_REQ_INIT);
                }
                ERROR;

            case ST_CHUNK_DATA:
                to_read = (ssize_t) p->content_length - p->content_read;
                to_read = MIN(p->bufend - p->buf, to_read);
                INIT_STATE(HTTP_BODY);
                if(to_read > 0)
                {
                    p->content_read += to_read;
                    p->buf += to_read;
                    SUSPEND(HTTP_BODY);
                    if(p->content_read == p->content_length)
                    {
                        JUMP(ST_CHUNK_DATA_ALMOST_DONE);
                    }
                }
                break;

            case ST_CHUNK_DATA_ALMOST_DONE:
                NEXT_IF(ch == CR, ST_CHUNK_DATA_DONE);
                NEXT_IF(ch == LF, ST_CHUNK_SIZE_START);
                ERROR;

            case ST_CHUNK_DATA_DONE:
                NEXT_IF(ch == LF, ST_CHUNK_SIZE_START);
                ERROR;
            
            default:
                goto error;

        }
    }

    if(p->buf == p->bufend && p->state == ST_BODY_IDENTITY_EOF)
    {
        return HTTP_BODY_EOF;
    }
    return p->action;

error:
    return HTTP_ERROR;
}
