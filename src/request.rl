
#include <assert.h>
#include <string.h>

#include "request.h"

%%{
    machine http_req_parser;

    action mark {
        assert(parser->mark == NULL && "won't overwrite a mark.");
        parser->mark = fpc;
    }

    action write_method {
        write_field(parser, &(parser->method), fpc);
    }

    action write_scheme {
        write_field(parser, &(parser->scheme), fpc);
    }

    action write_host {
        write_field(parser, &(parser->host), fpc);
    }

    action mark_port {
        parser->port = 0;
    }
    
    action write_port {
        parser->port = parser->port*10 + ((*fpc)-'0');
    }

    action write_path {
        write_field(parser, &(parser->path), fpc);
    }

    action write_query {
        write_field(parser, &(parser->query), fpc);
    }

    action write_fragment {
        write_field(parser, &(parser->fragment), fpc);
    }

    action mark_uri {
        assert(parser->mark_uri != NULL && "wont overwrite uri mark");
        parser->mark_uri = fpc;
    }

    action write_uri {
        write_uri(parser, fpc);
    }

    action start_major {
        parser->vsn_major = 0;
    }
    
    action write_major {
        parser->vsn_major = parser->vsn_major*10 + ((*fpc)-'0');
    }
    
    action start_minor {
        parser->vsn_minor = 0;
    }
    
    action write_minor {
        parser->vsn_minor = parser->vsn_minor * 10 + ((*fpc)-'0');
    }

    action mark_name {
        assert(parser->method != NULL && "method not set before header name");
        assert(parser->uri != NULL && "uri not set before header name");
        assert(parser->hdr_field == NULL && "header name already marked");
        assert(parser->hdr_value == NULL && "header value already marked");
        
        assert(parser->mark == NULL && "wont overwrite a mark");
        parser->mark = fpc;
    }
    
    action write_name {
        write_field(parser, &(parser->hdr_name), fpc);
    }
    
    action mark_value {
        assert(parser->hdr_name != NULL && "value must have a name");
        parser->mark = fpc;
    }
    
    action write_value {
        write_field(parser, &(parser->hdr_value), fpc);
        add_header(parser);
    }

    action done {
        parser->body = fpc;
    }

    include http "http.rl";
    
    main := request;
}%%

%% write data;

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
    %% write init;

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
    
    %% write exec;
    
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
