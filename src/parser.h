
#include <unistd.h>

#ifndef uchar
typedef unsigned char uchar;
#endif

#ifndef ushort
typedef unsigned short ushort;
#endif

enum ACTION {
    HTTP_ERROR=0,
    
    HTTP_CONTINUE,

    HTTP_METHOD,
    HTTP_SCHEME,
    HTTP_HOST,
    HTTP_PORT,
    HTTP_PATH,
    HTTP_QS,
    HTTP_FRAGMENT,

    HTTP_HEADER_FIELD,
    HTTP_HEADER_VALUE,
    HTTP_HEADERS_DONE,

    HTTP_BODY,

    HTTP_DONE
};

enum PARSER_TYPE {
    HTTP_REQUEST_PARSER,
    HTTP_RESPONSE_PARSER
};

typedef struct {
    ushort          action;
    const uchar*    data;
    size_t          length;

    const uchar*    method;
    ushort          status;

    ushort          vmajor;
    ushort          vminor;

    int             content_length;
    int             content_read;

    // Private
    char            is_request;
    ushort          state;
    ushort          stateact;
    const uchar*    stateptr;
    
    const uchar*    buf;
    const uchar*    bufend;
    
    size_t          hdr_index;
    ushort          hdr_state;
    ushort          hdr_value;

    uchar           flags;
    
} http_parser;

http_parser* http_init_parser(ushort type);
size_t http_get_buffer(http_parser* parser, const uchar** buf);
void http_set_buffer(http_parser* parser, const uchar* buf, size_t len);
ushort http_should_keep_alive(http_parser* parser);
ushort http_run_parser(http_parser* parser);
