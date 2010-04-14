/*
    Originally from Zed Shaw's Mongrel project.
*/

%%{
    machine http;

    ##
    ## HTTP PROTOCOL GRAMMAR
    ##
    
    # Line ending

    crlf = "\r\n";

    # Character classes

    lws = (" " | "\t");
    safe = ("$" | "-" | "_" | ".");
    extra = ("!" | "*" | "'" | "(" | ")" | ",");
    escape = ("%" xdigit xdigit);
    unreserved = (alpha | digit | safe | extra);
    uchar = (unreserved | escape);
    pchar = (uchar | "/" | ";" | ":" | "@" | "&" | "=");
    ctl = (cntrl | 127);
    tspecials = ( "(" | ")" | "<" | ">" | "@" | ","
                | ";" | ":" | "/" | "[" | "]" | "?"
                | "=" | "{" | "}" | " " | "\t" | "\\" | "\"");
    token = (ascii -- (ctl | tspecials));

    # A simple URI machine
    
    scheme = (alpha | digit | "+" | "-" | ".")+ >mark %write_scheme;
    hostchars = (uchar | ";" | "?" | "&" | "=");
    hostpart = hostchars >mark %write_host;
    portpart = (":" (digit+)? >mark_port $write_port)?;
    host = scheme "://" (hostpart portpart);
    path = ("/" pchar*) >mark %write_path;
    query = ("?" (pchar*) >mark %write_query);
    fragment = ("#" pchar*) >mark $write_fragment;
    uri = (host? path? query? fragment?);

    # HTTP version
    
    vsn_major = digit+ >start_major $write_major;
    vsn_minor = digit+ >start_minor $write_minor;
    version = "HTTP/" vsn_major "." vsn_minor;

    # Headers
    
    header_name = (token -- ":")+ >mark_name %write_name;
    header_value = any* >mark_value %write_value;
    header = header_name ":" lws* <: header_value :> crlf;

    # Requests

    method = (upper | digit | safe){1,20} >mark %write_method;
    resource = ("*" | uri) >mark_uri %write_uri;
    request_line = method lws+ resource lws+ version lws* crlf;
    request = request_line header* (crlf @done);
    
    # Responses - Hammer this out later.
    # 
    # status_code = digit{3};
    # status_mesg = (text -- crlf)*;
    # response_line = version lws+ status_code lws+ status_mesg lws* crlf;
    # response = response_line header* (crlf @done);

}%%