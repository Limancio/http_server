#include "http_server.h"

#define SHOW_DEBUG_REQUEST_INFO 1

internal void
add_work_entry(work_queue *work_queue, void* data) { // @TODO: Checks if is enought space in the queue to add new entries.
    work_queue_entry* entry = work_queue->work_entries + work_queue->next_entry_to_write;
    entry->data = *((net_socket*) data); // @NOTE: Temporary casting to SOCKET
    work_queue->completion_goal += 1;

    write_barrier();
    _mm_sfence();
    
    work_queue->next_entry_to_write = (work_queue->next_entry_to_write + 1) % 256; // @temporary: Array_count

#if _WIN32
    thread_mutex_unlock(work_queue->mutex_handle, 1, 0);
#else
    thread_mutex_unlock(&work_queue->mutex_handle.mutex);
#endif
}

internal void
do_worker_work(work_queue_entry entry, string_bucket* string_list, thread_work_info* info) {
    string_clear_data(info->recv_string);
    string_clear_data(info->send_string);
    
    net_socket socket = entry.data;
    handle_request_socket(info->server_data_pointer, string_list, info->recv_string, info->send_string, info->logical_index, socket);
    
    close_socket(socket);
}

internal b32
do_next_work_queue_entry(work_queue* work_queue, string_bucket* string_list,thread_work_info* info) {
    b32 should_sleep = false;

    u32 index;
    u32 start_next_entry_to_read = work_queue->next_entry_to_read;
    if(start_next_entry_to_read != work_queue->next_entry_to_write) {
#if _WIN32
        u32 new_next_entry_to_read = (start_next_entry_to_read + 1) % 256; // @temporary: Array_count 
        index = interlocked_compare_exchange((LONG volatile *) &work_queue->next_entry_to_read,
                                             new_next_entry_to_read,
                                             start_next_entry_to_read);
#else
        index = (u32) interlocked_compare_exchange((u32 volatile *) &work_queue->next_entry_to_read, 1);
#endif
        
        if(index == start_next_entry_to_read) {
            work_queue_entry entry = work_queue->work_entries[index]; 
            do_worker_work(entry, string_list, info);
            // InterlockedIncrement((LONG volatile *) &work_queue->completion_count);            
        }
    } else {
        should_sleep = true;
    }
    
    return(should_sleep);
}

internal void
push_socket(work_queue *work_queue, net_socket* socket) {
    add_work_entry(work_queue, socket);
}

THREAD_FUNCTION
thread_request_handler(void *parameter) {
    thread_work_info* info = reinterpret_cast<thread_work_info*>(parameter);
    string_bucket* string_list = NULL;
    
    size_t MAX_BUFFER_SIZE = 1000000; // @incomplete
    string_alloc(string_list, info->recv_string, MAX_BUFFER_SIZE);
    string_alloc(string_list, info->send_string, MAX_BUFFER_SIZE); 
    
    for(;;) {
        if(do_next_work_queue_entry(info->work_queue, string_list, info)) {
#if _WIN32
            thread_mutex_lock(info->work_queue->mutex_handle, INFINITE, FALSE);
#else
            thread_mutex_lock(&info->work_queue->mutex_handle.mutex);
#endif
        }
    }
    
    string_free(string_list, info->recv_string);
    string_free(string_list, info->send_string);
    return(0);
}

THREAD_FUNCTION
thread_update_handler(void *parameter) {
    thread_update_info* info = reinterpret_cast<thread_update_info*>(parameter);
    
    server_data *&server_data   = info->server_data_pointer; 
    string_bucket *&string_list = server_data->fetch_string_list;
    fetch_hash *&hash_map       = server_data->fetch_map;
    mutex_handle mutex_handle   = server_data->fetch_mutex_handle;
    
    const s64 UPS = 10;
    const s64 ms_ups = 1000 / UPS;
    
    ticks performance_frequency;
    ticks last_counter;
    ticks elapsed_counter;
    
    s64 perf_count_frequency;
    
    get_ticks(&performance_frequency);
    get_ticks(&last_counter);
    
#if _WIN32
    perf_count_frequency = (s64) performance_frequency.QuadPart;
#else
    perf_count_frequency = (s64) performance_frequency;
#endif
    
    s64 elapsed      = 0;
    s64 ms_per_frame = 0;
    for(;;) {        
        u32 start_value = server_data->fetch_on_update;
        
        if(start_value == 0) {
            u32 index;
            
#if _WIN32
            u32 new_value = 1;
            index = interlocked_compare_exchange((LONG volatile *) &server_data->fetch_on_update, new_value, start_value);
#else
            index = (u32) interlocked_compare_exchange((u32 volatile *) &server_data->fetch_on_update, 1);
#endif
            
            if(index == start_value) {
                u32 length = (u32) shlen(hash_map);
                for(u32 i = 0; i < length; i++) {
                    fetch_value* value = &hash_map[i].value;
                    value->elapsed_time += (s32) ms_per_frame;
                    // log("%i/%i\n", value->elapsed_time, value->refresh_time);
                    
                    if(value->elapsed_time >= value->refresh_time) {
                        value->elapsed_time = 0;
                        
                        clear_fetch_value(string_list, value);
                        if(value->refresh_time > 0) {
                            handle_fetch_request(value, string_list);
                        } else {
                            string_free(string_list, value->fetch_string);
                            shdel(hash_map, hash_map[i].key); // @temporary
                        }
                    }
                }  
                
                write_barrier();
                _mm_sfence();
                
#if _WIN32
                thread_mutex_unlock(mutex_handle, 1, 0);
#else
                thread_mutex_unlock(&mutex_handle.mutex);
#endif
                server_data->fetch_on_update = 0;
            }
        }

        get_ticks(&elapsed_counter);
#if _WIN32
        elapsed = (s64) (elapsed_counter.QuadPart - last_counter.QuadPart);
#else
        elapsed = (s64) (elapsed_counter - last_counter);
#endif
        ms_per_frame = (s32)((1000 * elapsed) / perf_count_frequency);
        
        s64 sleep_time = ms_ups - ms_per_frame;
        sleep_time = (sleep_time > 0) ? sleep_time : 0;

#if _WIN32
        Sleep((DWORD) sleep_time);
#else
        usleep(sleep_time);
#endif
        
        ms_per_frame += sleep_time;
        
        get_ticks(&elapsed_counter);
        last_counter = elapsed_counter;
    }
    
    return(0);
}

internal void
print_hash(meow_u128 hash) {
    printf("    %08X-%08X-%08X-%08X\n",
           MeowU32From(hash, 3),
           MeowU32From(hash, 2),
           MeowU32From(hash, 1),
           MeowU32From(hash, 0));
}

internal void
start_http_server(server_data* server_data, s8* private_ip_address, s8* public_ip_address){
    {
        s32 cpu_type;
        s32 cpu_count;
        s32 page_size;

#if _WIN32
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        
        cpu_type = sysinfo.dwProcessorType;
        cpu_count = sysinfo.dwNumberOfProcessors;
        page_size = sysinfo.dwPageSize;        
#else
        cpu_type = -1;
        cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
        page_size = sysconf(_SC_PAGE_SIZE);
#endif
        
        
        log("---===SERVER INFO===---\n\n");
        log("   CPU type: %i\n", cpu_type);
        log("   CPU core count: %i\n", cpu_count);
        log("   Page Size: %i\n", page_size);

        u32 main_thread = 1;
        u32 update_thread = 1;

        cpu_count = 3;
        
        server_data->thread_count = cpu_count - (main_thread - update_thread);
    }

#if 0 // hashing tool    
    char* buffer_data = "password_to_do_hash";
    meow_u128 hash_a = MeowHash(MeowDefaultSeed, sizeof(buffer_data), buffer_data);
    meow_u128 hash_b;
    memcpy(&hash_b, &hash_a, sizeof(hash_a));

    if(MeowHashesAreEqual(hash_a, hash_b)) {
        log("equals!\n")
    } else {
        log("not equals!\n")
    }
    print_hash(hash_a);
    print_hash(hash_b);
#endif
    
    database_data* database = &server_data->database_data;
    database_init_and_connect(database);
    
#if _WIN32    
    WORD version_requested;
    WSADATA wsa_data;
    
    version_requested = MAKEWORD(2, 2);
    s32 start_err = WSAStartup(version_requested, &wsa_data);
    if(start_err != 0) {
        log_error("WSAStartup failed with error: %d\n", start_err);
        exit(1);
    }
    
    if(LOBYTE(wsa_data.wVersion) != 2 || HIBYTE(wsa_data.wVersion) != 2) {
        log_error("Could not find a usable version of Winsock.dll\n");
        WSACleanup();
        exit(1);
    }
#endif
    
    net_socket server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket < 0) {
        log_error("Socket error. Probably another socket is using this port.\n");
        exit(1);
    }
  
    sockaddr_in server_address = {};
    server_address.sin_family  = AF_INET;
    server_address.sin_port    = htons(U32_BIND_PORT);     
    server_address.sin_addr.s_addr = INADDR_ANY;

#if 1
    if(inet_pton(AF_INET, private_ip_address, &server_address.sin_addr) <= 0) {
        log_error("Invalid address/ Address not supported \n");
        exit(1);
    }
#endif
    
    s32 bind_result = bind(server_socket, (net_socket_address*) &server_address, sizeof(server_address));
    if(bind_result == -1) {
        log_error("Bind error.\n");
        exit(1);
    }
  
    s32 listening = listen(server_socket, BACKLOG);
    if(listening < 0) {
        log_error("Error: The server is not listening.\n");
        exit(1);
    }
  
    {
        char host_buffer[INET6_ADDRSTRLEN];
        char service_buffer[NI_MAXSERV]; // defined in `<netdb.h>`
        
        socklen_t addr_len = sizeof(server_address);
        s32 result = getnameinfo((net_socket_address*) &server_address,
                                 addr_len,
                                 host_buffer,
                                 sizeof(host_buffer),
                                 service_buffer,
                                 sizeof(service_buffer),
                                 NI_NUMERICHOST
                                 );
        
        if(result != 0) {
            log_error("Server is not working.\n");
        } else {
            log("\n\n\tServer listening on http://%s:%s\n", host_buffer, service_buffer);
            
            string temp_string = static_to_string(server_data->string_url_path);
            string_append(temp_string, const_string("//"));
            string_append(temp_string, public_ip_address, strlen(public_ip_address));
            string_append(temp_string, const_string(":"));
            string_append(temp_string, HTTP_URL_PORT);
            update_static_string(server_data->string_url_path, temp_string);
        }
    }
    
    u32 initial_count = 0;
    u32 max_count = server_data->thread_count;
    
    work_queue* work_queue = &server_data->work_queue;

#if _WIN32
    work_queue->mutex_handle = thread_mutex_init(0, initial_count, max_count, 0, 0, SEMAPHORE_ALL_ACCESS);
#else
    thread_mutex_init(&work_queue->mutex_handle.condition, &work_queue->mutex_handle.mutex);
#endif
    
    thread_work_info thread_work_info_array[32] = {}; // @temporary
    for(u32 index = 0; index < server_data->thread_count; index++) {
        thread_work_info* thread_info = &thread_work_info_array[index];
        
        thread_info->logical_index = index;
        thread_info->work_queue    = work_queue;
        thread_info->server_data_pointer = server_data;
        
#if _WIN32
        DWORD thread_id;
        server_data->thread_array[index] = create_thread(0, 0, thread_request_handler, thread_info, 0, &thread_id);
#else
        create_thread(&server_data->thread_array[index], NULL, thread_request_handler, thread_info);
#endif
    }
    
    thread_update_info update_info = {};
#if _WIN32
    server_data->fetch_mutex_handle = thread_mutex_init(0, initial_count, max_count, 0, 0, SEMAPHORE_ALL_ACCESS);
#else
    thread_mutex_init(&server_data->fetch_mutex_handle.condition, &server_data->fetch_mutex_handle.mutex);
#endif
    
    { // init_thread:
        u32 index = server_data->thread_count;
        thread_update_info* thread_info = &update_info;
        
        thread_info->logical_index = index;
        thread_info->server_data_pointer = server_data;
                
#if _WIN32
        DWORD thread_id;
        server_data->thread_array[index] = CreateThread(0, 0, thread_update_handler, thread_info, 0, &thread_id);        
#else
        create_thread(&server_data->thread_array[index], NULL, thread_update_handler, thread_info);
#endif
    }
    
    for(;;) {
        net_socket client_socket = accept(server_socket, 0, 0);
        
        if(client_socket != -1) {
            push_socket(work_queue, &client_socket);
        }
    }
    
    // @incomplete: cleanup
    // CloseHandle(work_queue->semaphore_handle);
    // WaitForMultipleObjects(MAX_THREADS, thread_array, TRUE, INFINITE);
    
    database_close(database);
    int close_result = close_socket(server_socket);
    // WSACleanup();
}
