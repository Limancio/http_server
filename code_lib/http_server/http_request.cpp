
internal b32
ready_to_recv(net_socket socket, s32 interval) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(socket, &fds);
    
    timeval tv = {interval, 0};
    return(select((s32) socket + 1, &fds, NULL, NULL, &tv) == 1);
}

internal string
get_fetch_json_data(string string_data) {
    string result = {};
    const string POST_CONTENT_TYPE_LENGTH = const_string("Content-Length:");
  
    reader_file reader = {};
    init_reader(&reader, string_data.data, string_data.count);
    string line;
    do {
        get_next_line(line, &reader, false);
    
        if(string_starts_with(line, POST_CONTENT_TYPE_LENGTH)) {
            string string_length = string_substring_at(line, POST_CONTENT_TYPE_LENGTH.count + 1);
            u32 length = string_to_u32(string_length);
      
            result.count = length;
            result.data_size = length;
        } else if(!string_empty(line) && !string_contents(line)) {
            get_next_line(line, &reader, false);
            result.data = line.data;
        }
    } while(!string_empty(line));
  
    return(result);
}

internal void
handle_fetch_request(fetch_value *fetch_value, string_bucket *&string_list) {
    string url_request = static_to_string(fetch_value->fetch_url);
    string host_name = string_get_substring(url_request, '/');
    string request = string_get_substring(string{}, '\n');
    
    string complete_request;
    string_alloc(string_list, complete_request, HTTP_HEADER_FETCH);
    string_replace(string_list, complete_request, complete_request, const_string("%request%"), request);
    string_replace(string_list, complete_request, complete_request, const_string("%host_name%"), host_name);
    
    s32 s32_result = 0;
    u32 port       = 80; // @temporary
    net_socket sock = socket(AF_INET, SOCK_STREAM, 0); // IPPROTO_TCP
    if(sock < 0) {
        log_error("\n Socket creation error \n");
        exit(1);
    }
    
    addrinfo* info  = NULL;
    addrinfo  hints = {};
    sockaddr_in server_address;
    
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    string host;
    string_alloc(string_list, host, host_name);
    
    s32_result = getaddrinfo(host.data, NULL, &hints, &info);
    string_free(string_list, host);
    if(s32_result != 0) {
        // log
    }
  
    server_address = *(sockaddr_in*) (info->ai_addr);
    server_address.sin_port = htons(port);
    
    s32_result = connect(sock, (net_socket_address*) (&server_address), sizeof(server_address));
    if(s32_result != 0) {
        log_error("Connection error.\n");
        exit(1);
    }
  
    s32_result = send(sock, complete_request.data, (s32) complete_request.count, 0);
    if(s32_result == -1) {
        log_error("send error.\n");
    }
    string_free(string_list, complete_request);
    
    b32 do_recv = ready_to_recv(sock, 1);
    if(do_recv) {
        string& fetch_string = fetch_value->fetch_string;
        s32 recv_size = 0, recv_result = 0;
    
        do {
            recv_result = recv(sock, &fetch_string.data[recv_size], (s32) fetch_string.data_size - recv_size, 0);
            recv_size += recv_result;
        }while(recv_result != 0);
    
        fetch_string.data[recv_size] = '\n';
        fetch_string.count = recv_size + 1;
    
        string json_data = get_fetch_json_data(fetch_string);
        read_json_data(&fetch_value->element_list_arena, &fetch_value->root_element, json_data);
    }
    close_socket(sock);
    
    write_barrier();
    _mm_sfence();
    
    fetch_value->ready = true;
}

internal void
clear_fetch_value(string_bucket* string_list, fetch_value* value) {
    memset(value->fetch_string.data, 0, value->fetch_string.count);
    value->fetch_string.count = 0;
            
    clear_element_list(value->element_list_arena.bucket_list);
    value->element_list_arena = {};
    value->root_element = {};
}

internal fetch_value
get_fetch_url(server_data *server_data, string fetch_url, s32 refresh_time) {
    fetch_value new_value = {};
    string_bucket *&string_list = server_data->fetch_string_list; // @temporary

    while(!new_value.ready) {
        
        u32 start_value = server_data->fetch_on_update;
        if(start_value > 0) {
#if _WIN32
            thread_mutex_lock(server_data->fetch_mutex_handle, INFINITE, FALSE);
#else
            thread_mutex_lock(&server_data->fetch_mutex_handle.mutex);
#endif
        }
        start_value = server_data->fetch_on_update;
        u32 index;
        u32 set_value = 1;

#if _WIN32
        index = interlocked_compare_exchange((LONG volatile *) &server_data->fetch_on_update,
                                                 set_value,
                                                 start_value);
#else
        index = (u32) interlocked_compare_exchange((u32 volatile *) &server_data->fetch_on_update, 1);
#endif

        if(index == start_value && start_value == 0) {
            s32 index_key = (s32) shgeti(server_data->fetch_map, fetch_url.data);
            if(index_key < 0) {
                string_alloc(string_list, new_value.fetch_string, 4096 * 10);
                string_alloc(string_list, new_value.fetch_url, fetch_url);
        
                handle_fetch_request(&new_value, string_list);
                new_value.refresh_time = refresh_time;
        
                shput(server_data->fetch_map, new_value.fetch_url.data, new_value);
            } else {
                new_value = server_data->fetch_map[index_key].value;

                if(new_value.refresh_time <= 0) {
                    clear_fetch_value(string_list, &new_value);
                    handle_fetch_request(&new_value, string_list);
                }
            }
                            
            write_barrier();
            _mm_sfence();
            
            server_data->fetch_on_update = 0;
        }
    }
            
    return(new_value);
}

internal void
handle_request_socket(server_data* server_data, string_bucket *&string_list, string &recv_string, string &send_string, u32 thread_index, net_socket socket) {
    string string_url_path = static_to_string(server_data->string_url_path);
    
    b32 handle_request = ready_to_recv(socket, 1);
    if(handle_request) {
        recv_string.count = recv(socket, recv_string.data, (s32) recv_string.data_size, 0);
        
        if(recv_string.count > 0) {
#if 0
            {
                string header = string_get_substring(recv_string, '\r');
                
                log("request from thread [%i]: ", thread_index);
                log_string(header);
                log("\n");
            }
#endif
            
            string request_value; // @NOTE: must be free.
            string method = string_get_substring(recv_string, ' ');
            string request_complete_value = string_get_substring(string{}, ' ');
            
            {
                string temp_string = string_get_substring(request_complete_value, '?');
                string_alloc(string_list, request_value, temp_string);
            }
            
            b32 handle_source_request    = false;
            b32 handle_request_not_found = false;
            request_data request_data    = {};
            
            // @temporary:
            request_type request_method;
            size_t html_data_size = 0;
            string response_type  = {};
            
            if(string_equals(method, const_string("POST"))) {
                handle_source_request = true;
                
                response_type = const_string("text/html");
                request_method = POST_REQUEST;
            } else if(string_equals(method, const_string("GET"))) {
                if(file_type_equals(request_value, const_string(".css"))) {
                    response_type = const_string("text/css");
                } else if(file_type_equals(request_value, const_string(".png"))) {
                    response_type = const_string("image/png");
                } else if(file_type_equals(request_value, const_string(".jpg"))) {
                    response_type = const_string("image/jpg");
                } else if(file_type_equals(request_value, const_string(".svg"))) {
                    response_type = const_string("image/svg+xml\r\nVary: Accept-Encoding");
                } else if(file_type_equals(request_value, const_string(".ico"))) {
                    response_type = const_string("image/image");
                } else {
                    handle_source_request = true;
                    response_type = const_string("text/html");
                    request_method = GET_REQUEST;
                }
            }
            request_data = parse_request_data(request_method, string_list, request_complete_value, recv_string);
            
            string_append(send_string, HTTP_HEADER_RESPONSE);
            if(handle_source_request) {
                string module_path = {};
                
                string_alloc(string_list, module_path, STRING_STACK_MAX); // @temporary
                string_append(module_path, static_to_string(request_data.route_file_name));
                string_append(module_path, DIV_PATH_TOKEN);
                string_append(module_path, HTTP_PAGES_LIB_PATH);
                string_append(module_path, static_to_string(request_data.module_name));
                string_append(module_path, LIB_FILE_TYPE);
                
                request_data.module_handler = load_library(module_path.data);
                if(request_data.module_handler) {
                    string procedure_name = {};
                    string_alloc(string_list, procedure_name, static_to_string(request_data.internal_name));
                    
                    module_internal_proc proc = (module_internal_proc) get_procedure_address(request_data.module_handler, procedure_name.data);
                    if(proc) {
                        size_t header_count = send_string.count; 
                        proc(server_data, string_list, &request_data, send_string);
                        html_data_size = send_string.count - header_count;
                    } else {
                        handle_request_not_found = true;
                    }
                    
                    free_library(request_data.module_handler);
                    string_free(string_list, procedure_name);
                } else {
                    handle_request_not_found = true;
                }
                string_free(string_list, module_path);
                
                //@temporary: copy-pasta	    	    
                { // @incomplete: could replace html data.
                    string content_length_value;
                    string_alloc(string_list, content_length_value, 32);
                    
                    u32_to_string(content_length_value, (u32) html_data_size);
                    string_replace(string_list, send_string, send_string, const_string("%type%"), response_type); 
                    string_replace(string_list, send_string, send_string, const_string("%size%"), content_length_value);                      
                    string_free(string_list, content_length_value);
                    
                    // @temporary: 
                    if(!string_empty(request_data.extra_header_data)) {
                        string_replace(string_list, send_string, send_string, const_string("%extra-data%"), request_data.extra_header_data);  
                        string_free(string_list, request_data.extra_header_data);
                    } else {
                        string_replace(string_list, send_string, send_string, const_string("%extra-data%"), string{});  
                    }
                }
            } else {	    	    
                string public_file_path;
                string route_name = static_to_string(request_data.route_name);
                string route_file_name = static_to_string(request_data.route_file_name);
                
                string fixed_file_path;
                if(string_empty(route_name)) {
                    fixed_file_path = string_substring_at(request_value, 1);
                } else {
                    fixed_file_path = string_substring_at(request_value, route_file_name.count + 2);
                }
                string_concatenate(string_list, public_file_path, 4, route_file_name, DIV_PATH_TOKEN, HTTP_PUBLIC_PATH, fixed_file_path);

// string_concatenate(string_list, public_file_path, 2, HTTP_PUBLIC_PATH, request_value); // @temporary
                
                html_data_size = (size_t) (get_file_size(public_file_path));
	    	    
                { // @incomplete: could replace html data.
                    string content_length_value;
                    string_alloc(string_list, content_length_value, 32);
                    
                    u32_to_string(content_length_value, (u32) html_data_size);
                    string_replace(string_list, send_string, send_string, const_string("%type%"), response_type); 
                    string_replace(string_list, send_string, send_string, const_string("%size%"), content_length_value);  
                    string_free(string_list, content_length_value);
                    
                    // @temporary: 
                    if(!string_empty(request_data.extra_header_data)) {
                        string_replace(string_list, send_string, send_string, const_string("%extra-data%\r\n"), request_data.extra_header_data);  
                    } else {
                        string_replace(string_list, send_string, send_string, const_string("%extra-data%\r\n"), string{});  
                    }
                }
                
                read_file(public_file_path, &send_string.data[send_string.count], html_data_size);
                string_free(string_list, public_file_path);
                send_string.count += html_data_size;
            }
            
            if(handle_request_not_found) {
                memset(send_string.data, 0, send_string.count);
                send_string.count = 0;
                
                string_append(send_string, HTTP_HEADER_404_RESPONSE); // @temporary
                html_data_size = send_string.count;
            }
            
            s32 send_result = send(socket, send_string.data, (s32) send_string.count, 0);
            if(send_result == -1) {
                log_error("Send Failed.\n");
            }
            
            string_free(string_list, request_value); // @temporary
        }
    }
}
