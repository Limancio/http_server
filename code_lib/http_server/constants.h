#ifndef CONSTANTS_H
#define CONSTANTS_H

static const string HTTP_HEADER_CHANGE_URL   = const_string("HTTP/1.1 302 Found\r\n%extra-data%Location: %url%%route%\r\n\r\n");
static const string HTTP_HEADER_404_RESPONSE = const_string("HTTP/1.0 404 Not Found");
static const string HTTP_HEADER_RESPONSE     = const_string("HTTP/1.1 200 OK\r\nCache-Control: no-cache, no-store, must-revalidate\r\nConnection: close\r\nPragma: no-cache\r\nExpires: 0\r\n%extra-data%Content-Type: %type%\r\nContent-Length: %size%\r\n\r\n");
static const string HTTP_HEADER_FETCH = const_string("GET /%request% HTTP/1.1\r\nHost: %host_name%\r\nConnection: close\r\n\r\n");

static const s32    U32_BIND_PORT = 80;
static const string HTTP_URL_PORT = const_string("80");

static const string HTTP_URL_PATH_NAME 	       = const_string("URL_PATH");
static const string HTTP_IMG_PATH_NAME 	       = const_string("IMG_PATH");

static const string HTTP_URL_API_NAME 	       = const_string("api");
static const string HTTP_DEFAULT_ROUTE_NAME    = const_string("default"); 
static const string HTTP_DEFAULT_MODULE_NAME   = const_string("index"); 
static const string HTTP_DEFAULT_INTERNAL_NAME = const_string("index_page"); 

static const string HTTP_PAGES_LIB_PATH        = const_string("pages_lib/");
static const string HTTP_PUBLIC_PATH 	       = const_string("public/");
static const string HTTP_PUBLIC_DATA_PATH      = const_string("public/data/");
static const string HTTP_PUBLIC_IMG_PATH       = const_string("public/img/");

static const string IMG_PATH = const_string("img");
static const string DIV_PATH_TOKEN = const_string("/");

#if _WIN32
static const string LIB_FILE_TYPE = const_string(".dll");
#else
static const string LIB_FILE_TYPE = const_string(".so");
#endif
#endif // !CONSTANTS_H
