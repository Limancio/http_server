#include "http_server/http_server.h"

extern "C"
{    
    HTML_FUNCTION(index_page) {
        LOAD_HTML(const_string("header.html"));
        LOAD_HTML(const_string("index.html"));
        LOAD_HTML(const_string("footer.html"));
    }
}