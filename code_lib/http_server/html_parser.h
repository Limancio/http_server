#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include "http_server.h"

// @temporary macro functions:
#define token_replace(_string, _token, _value) string_replace(string_list, _string, _string, const_string(_token), _value)
#define http_change_url(_new_url)                                       \
    string_clear_data(html_content);                                    \
    string __temp_new_url = const_string(_new_url);                     \
    string __temp_route_string = static_to_string(request_data->route_file_name); \
    string_append(html_content, HTTP_HEADER_CHANGE_URL);                \
    token_replace(html_content, "%url%", __temp_new_url);               \
    token_replace(html_content, "%route%", __temp_route_string);
    
// #define HTML(_html_code) const_string("" #_html_code "") 
// #define INNER_HTML(_html_content)  parse_html_content(server_data, string_list, request_data, _html_content)

// #define REPLACE_HTML(_token, _value) string_replace(string_list, html_content, html_content, const_string("" #_token ""), _value)
#define REPLACE_HTML(_token_value) string_replace(string_list, html_content, html_content, const_string("{" #_token_value "}"), _token_value)

#define INNER_HTML(__html_code) string_append(html_content, const_string("" #__html_code ""))
#define LOAD_HTML(_html_page_path) read_html_file(server_data, string_list, request_data, _html_page_path, html_content)

typedef void(/*WINAPI */ *module_internal_proc)(server_data *, string_bucket *, request_data *, string &);

#define HTML_FUNCTION(_internal_name) MODULE_EXPORT void _internal_name(server_data *server_data, string_bucket * string_list, request_data *request_data, string &html_content)
#define CALL_HTML_FUNCTION(_internal_name) _internal_name(server_data, string_list, request_data, html_content)

#endif // !HTML_PARSER_H
