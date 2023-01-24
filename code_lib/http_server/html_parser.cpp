#include "html_parser.h"

// @temporary: @speedup: Retrieve GET and POST data in a way. Is not the optimal and best way.
internal request_data
parse_request_data(request_type request_type, string_bucket* string_list, string get_string, string recv_string) {
    request_data request_data = {};
    
    { // @temporary:
        const string COOKIES_DATA_ARG = const_string("Cookie:");
        reader_file reader = {};
        init_reader(&reader, recv_string.data, recv_string.count);
        
        string line;
        do {
            get_next_line(line, &reader);
            
            if(string_starts_with(line, COOKIES_DATA_ARG)) {
                string ignore = string_get_substring(line, ' '); 
                string token;
                while(!string_empty(token = string_get_substring(string{}, ' '))) {
                    u32 index = request_data.cookies_array_count;
                    get_data* cookie_data = &request_data.cookies_array[index];
                    request_data.cookies_array_count += 1; 
                    
                    cookie_data->key   = string_get_substring(token, '=');
                    if(string_contains(cookie_data->key, const_string(";"))) {
                        cookie_data->key.count -= 1;
                        cookie_data->value = string_get_substring(string{}, ' '); 
                    } else {
                        cookie_data->value = string_get_substring(string{}, '\n'); 
                        cookie_data->value.count -= 1;
                    }
                }
            }
        } while(!string_empty(line));
    }
    
    if(get_string.count == 1) {    
        static_string_append(request_data.route_name, string{});
        static_string_append(request_data.route_file_name, HTTP_DEFAULT_ROUTE_NAME);
        
        static_string_append(request_data.module_name,   HTTP_DEFAULT_MODULE_NAME);
        static_string_append(request_data.internal_name, HTTP_DEFAULT_INTERNAL_NAME);
        
        return(request_data);
    }
    
    if(get_string.data[get_string.count - 1] == '/') {
        get_string.count -= 1;
    }
    
    enum request_parse_type {PARSE_ROUTE = 0, PARSE_MODULE, PARSE_INTERNAL, PARSE_API, PARSE_ARGS, PARSE_GET_KEY, PARSE_GET_VALUE};
    request_parse_type parse_cursor = PARSE_ROUTE;
    get_data* active_get_value = NULL;
    
    lexer lexer = {};
    lexer_init(&lexer, get_string.data, &get_string.data[get_string.count]);
    while(lexer_get_token(&lexer)) {
        if(lexer.token == LEX_PARSE_ERROR) {
            log_error("PARSE ERROR\n");
            break;
        }
        
        if(lexer.token == '/' || lexer.token == '?' || lexer.token == '&') {
            if(lexer.token != '/' || (lexer.token == '/' && parse_cursor == PARSE_ARGS)) {
                parse_cursor = PARSE_GET_KEY;
                
                u32 index = request_data.array_count;
                active_get_value = &request_data.get_data_array[index];
                request_data.array_count += 1; 
            }
        } else if(lexer.token == '=') {
            parse_cursor = PARSE_GET_VALUE;
            lexer.parse_flags = PARSE_URL_VALUES;
        }
        
        if(lexer.token == LEX_ID || lexer.token == LEX_STRING) {
            if(parse_cursor == PARSE_ROUTE) {
                string route_folder_path;
                string_alloc(string_list, route_folder_path, lexer.string_token);
                // string_concatenate(string_list, route_folder_path, 2, DIV_PATH_TOKEN, lexer.string_token);

                
                if(file_exists(route_folder_path)) {
                    static_string_append(request_data.route_name, route_folder_path);
                    static_string_append(request_data.route_file_name, route_folder_path);
                    parse_cursor = PARSE_API;
                    continue;
                }
                
                string_free(string_list, route_folder_path);
            }
            if(parse_cursor == PARSE_API) {
                parse_cursor = PARSE_MODULE;
                if(string_equals(lexer.string_token, HTTP_URL_API_NAME)) {
                    request_data.api_call = true;
                    continue;
                }
            }
            
            if(parse_cursor == PARSE_MODULE) {
                static_string_append(request_data.module_name, lexer.string_token);
                parse_cursor = PARSE_INTERNAL;
            } else if(parse_cursor == PARSE_INTERNAL) {
                static_string_append(request_data.internal_name, lexer.string_token);
                parse_cursor = PARSE_ARGS;
            } else if(parse_cursor == PARSE_GET_KEY) {
                active_get_value->key = lexer.string_token;
                parse_cursor = PARSE_ARGS;
            } else if(parse_cursor == PARSE_GET_VALUE) {
                active_get_value->value = lexer.string_token;
                
                // @TODO: replace for get_value_to_string
                string_replace(string_list, active_get_value->value, active_get_value->value, const_string("%20"), const_string(" "));
                parse_cursor = PARSE_ARGS;
                lexer.parse_flags = 0;
            }
        }
    }
    
    // @temporary:
    if(string_empty(static_to_string(request_data.route_name)) && string_empty(static_to_string(request_data.route_file_name))) {
        // static_string_append(request_data.route_name, HTTP_DEFAULT_ROUTE_NAME);
        static_string_append(request_data.route_file_name, HTTP_DEFAULT_ROUTE_NAME);
    }
    if(string_empty(static_to_string(request_data.module_name))) {
        static_string_append(request_data.module_name, HTTP_DEFAULT_MODULE_NAME);
    }
    if(string_empty(static_to_string(request_data.internal_name))) {
        static_string_append(request_data.internal_name, HTTP_DEFAULT_INTERNAL_NAME);
    }
    
    if(request_type == POST_REQUEST) {
        reader_file reader = {};
        init_reader(&reader, recv_string.data, recv_string.count);
        
        // @temporary: const_string values.
        const string POST_CONTENT_DISPOSITION_ARG = const_string("Content-Disposition:");
        const string POST_CONTENT_TYPE_ARG = const_string("Content-Type:");
        
        b32 parse_data_line   = false;
        get_data* active_data = NULL;
        
        string form_label = {}; // @temporary: Must be free.
        string end_form_label = {}; // @temporary: Must be free.
        
        string line;
        do {
            get_next_line(line, &reader, false);
            string label = string_get_substring(line, '\r');
      
            if(!string_empty(form_label)) {
                if(string_equals(label, form_label)) {
                    parse_data_line = false;

                    active_data = &request_data.get_data_array[request_data.array_count];
                    request_data.array_count += 1;
                } else if(string_starts_with(label, POST_CONTENT_DISPOSITION_ARG)) { // @temporary:
                    string ignore_token = string_get_substring(label, ' ', true); 
                    string data_path = string_get_substring(string{}, ' '); // @note: Ignored value.
                    string key_value;
	  
                    do {
                        key_value = string_get_substring(string{}, ' '); // @note: Ignored value.
	    
                        // @temporary: const_string values.
                        const string FORM_NAME_ARG = const_string("name=");
                        const string FORM_FILE_NAME_ARG = const_string("filename=");
	    
                        if(string_starts_with(key_value, FORM_NAME_ARG)) {
                            active_data->key = string_substring_at(key_value, FORM_NAME_ARG.count);
	      
                            // @temporary: removing ';' from string
                            if(string_contains(active_data->key, const_string(";"))) {
                                active_data->key.count -= 2;
                            }
                            
                            string_clear_empty(active_data->key);
                            string_remove_quotes(active_data->key);
                        } else if(string_starts_with(key_value, FORM_FILE_NAME_ARG)) {
                            active_data->file_name = string_substring_at(key_value, FORM_FILE_NAME_ARG.count);

                            // @temporary: removing ';' from string
                            if(string_contains(active_data->file_name, const_string(";"))) {
                                active_data->file_name.count -= 1;
                            }
	      
                            string_clear_empty(active_data->file_name);
                            string_remove_quotes(active_data->file_name);
                        }
                    }while(!string_empty(key_value));

                    // @temporary: remove empty lines of values.
                    get_next_line(line, &reader, false);
                    parse_data_line = true;
                } else if(string_equals(label, end_form_label)) {
                    break;
                } else {
                    if(parse_data_line) {
                        string value = line;

                        if(string_empty(active_data->value)) {
                            string_clear_empty(value);
                            active_data->value = value;
                        } else {
                            active_data->value.count += value.count;
                            active_data->value.data_size = active_data->value.count;
                        }
                    }
                }
            } else {
                if(string_starts_with(label, POST_CONTENT_TYPE_ARG)) {
                    // @note: Ignore values
                    string ignore_token = string_get_substring(label, ' ', true); 
                    ignore_token = string_get_substring(string{}, ' ', true);
                    
                    // @temporary: const_string values.
                    string form_token = const_string("--");
                    const string FORM_BOUNDARY_ARG = const_string("boundary=");
                    
                    string temp_form_label = string_get_substring(string{}, ' ');
                    temp_form_label = string_substring_at(temp_form_label, FORM_BOUNDARY_ARG.count); // @temporary	
                    
                    string_concatenate(string_list, form_label,     2, form_token, temp_form_label);
                    string_concatenate(string_list, end_form_label, 2, form_label, form_token);
                }
            }
        } while(!string_empty(line));
      
        string_free(string_list, form_label);
        string_free(string_list, end_form_label);
    }

    return(request_data);
}

internal void
get_procedure_name(string &out_string, lexer* lexer, s8 *&proc_open_index, s8 *&proc_close_index) {
    u32 max_procedure_count = 32;
    
    if(proc_open_index == NULL && lexer->token == '{') {
        proc_open_index = lexer->reader.parse_point - 1;
    } else if(proc_close_index == NULL && lexer->token == '}') {
        proc_close_index = lexer->reader.parse_point - 1;
        size_t complete_size = proc_close_index - proc_open_index;
        
        if(complete_size > max_procedure_count) {
            proc_open_index  = NULL;
            proc_close_index = NULL;
            return;
        }
        
        out_string = cast_string(proc_open_index + 1, complete_size - 1, complete_size - 1);
        
        if(string_contains(out_string, const_string("()"))) {
            out_string.count     -= 2;
            out_string.data_size -= 2;
        }
        proc_close_index = lexer->reader.parse_point;
    }
}

internal void
get_load_html_value(string &out_string, string in_string) {
    s8 *open_index  = NULL;
    s8 *close_index = NULL;
    
    for(size_t i = 0; i < in_string.count; i++) {
        s8* token = &in_string.data[i];
        
        if(*token == '(') {
            // {value("url_path")}
            //       ^ 
            //       --^
            
            open_index = token + 2;
        } else if(*token == ')') {
            // {value("url_path")}
            //                  ^ 
            //                 ^-

            close_index = token - 1;
            size_t complete_size = close_index - open_index;
            
            out_string = cast_string(open_index, complete_size, complete_size);
        }
    }
}

internal size_t
read_html_file(server_data *server_data, string_bucket* string_list, request_data *request_data, string html_page_path, string &html_content);

internal void
parse_html_content(server_data *server_data, string_bucket *string_list, request_data *request_data, string &html_content) {
    string host_url_path = static_to_string(server_data->string_url_path);

    string route_name = static_to_string(request_data->route_name);
    string route_file_name = static_to_string(request_data->route_file_name);
    
    lexer lexer = {};
    lexer_init(&lexer, html_content.data, &html_content.data[html_content.count - 1]);
    
    string cpp_procedure_name = {};
    s8* proc_open_index  = NULL;
    s8* proc_close_index = NULL;
    while(lexer_get_token(&lexer)) {
        if(lexer.token == LEX_PARSE_ERROR) {
            log_error("PARSE ERROR\n");
            break;
        }

        get_procedure_name(cpp_procedure_name, &lexer, proc_open_index, proc_close_index);
        if(!string_empty(cpp_procedure_name)) {
            u32 open_at  = (u32) (proc_open_index - html_content.data);
            u32 close_at = (u32) (proc_close_index - html_content.data);

            if(string_equals(cpp_procedure_name, HTTP_URL_PATH_NAME)) {
                string route_url_path;
                if(string_empty(route_name)) {
                    string_alloc(string_list, route_url_path, host_url_path);
                } else {
                    string_concatenate(string_list, route_url_path, 3, host_url_path, DIV_PATH_TOKEN, route_name);
                }
                string_replace_at(html_content, open_at, close_at, route_url_path);
                
                string_free(string_list, route_url_path);
            } else if(string_equals(cpp_procedure_name, HTTP_IMG_PATH_NAME)) {
                string_replace_at(html_content, open_at, close_at, IMG_PATH);
            } else if(string_starts_with(cpp_procedure_name, const_string("LOAD_HTML"))) {
                string load_html_path;
                string complete_path;

                get_load_html_value(load_html_path, cpp_procedure_name);
                string_concatenate(string_list, complete_path, 4, route_file_name, DIV_PATH_TOKEN, HTTP_PUBLIC_PATH, load_html_path);
                
                string load_html_content; string_alloc(string_list, load_html_content, 4096 * 6); // @temporary
                s8* data_buffer = load_html_content.data;
                
                size_t html_file_size = (size_t) (get_file_size(complete_path));
                read_file(complete_path, data_buffer, html_file_size);
                load_html_content.count = html_file_size;
                
                parse_html_content(server_data, string_list, request_data, load_html_content);
                string_replace_at(html_content, open_at, close_at, load_html_content);
                
                string_free(string_list, load_html_content);
                string_free(string_list, complete_path);
            } else {
                string internal_name;
                
                dll_handle module_handler;
                module_internal_proc proc;
                
                b32 find_module = string_contains(cpp_procedure_name, const_string("::"));
                if(find_module) {
                    string module_name_temp   = string_get_substring(cpp_procedure_name, ':');
                    string internal_name_temp = string_get_substring(string{}, '\n');
                    internal_name_temp.data = &internal_name_temp.data[1];
                    internal_name_temp.count -= 1;
                    
                    string module_name;
                    string_alloc(string_list, module_name, module_name_temp);
                    string_alloc(string_list, internal_name, internal_name_temp);
                    
                    string route_name = static_to_string(request_data->route_name);
                    
                    string module_path;
                    string_concatenate(string_list, module_path, 5, route_name, DIV_PATH_TOKEN, HTTP_PAGES_LIB_PATH, module_name, LIB_FILE_TYPE);
                    
                    module_handler = load_library(module_path.data);
                    string_free(string_list, module_name);
                    string_free(string_list, module_path);
                } else {
                    module_handler = request_data->module_handler; 
                    string_alloc(string_list, internal_name, cpp_procedure_name);
                }                
                
                proc = (module_internal_proc) get_procedure_address(module_handler, internal_name.data);
                if(proc) {
                    string proc_html_content; string_alloc(string_list, proc_html_content, 4096 * 6); // @temporary
                    proc(server_data, string_list, request_data, proc_html_content);
                    
                    parse_html_content(server_data, string_list, request_data, proc_html_content);
                    
                    string_replace_at(html_content, open_at, close_at, proc_html_content);
                    string_free(string_list, proc_html_content);
                } else {
                    // TODO: Handle error.
                }
                
                string_free(string_list, internal_name);
                if(find_module) {
                    free_library(module_handler);
                }
            }

            proc_open_index  = NULL;
            proc_close_index = NULL;
            cpp_procedure_name = {};
        }
        
        // @TODO: when lexer.reader.buffer size is change we need to update eof.
        lexer.reader.eof = &html_content.data[html_content.count - 1];
    }
}

internal size_t
read_html_file(server_data *server_data, string_bucket* string_list, request_data *request_data, string html_page_path, string &html_content) {
    size_t new_content_size = 0;
    s8* data_buffer = &html_content.data[html_content.count];
    new_content_size = 0;
    
    string complete_path;

    string route_file_name = static_to_string(request_data->route_file_name);
    string_concatenate(string_list, complete_path, 4, route_file_name, DIV_PATH_TOKEN, HTTP_PUBLIC_PATH, html_page_path);
    
    size_t html_file_size = (size_t) (get_file_size(complete_path));
    read_file(complete_path, data_buffer, html_file_size);
    string_free(string_list, complete_path);
    
    string html_string = cast_string(data_buffer, html_file_size, html_file_size);
    parse_html_content(server_data, string_list, request_data, html_string);
    html_content.count += html_string.count; //@NOTE: posible overflow error.
    
    return(html_string.count);
}

