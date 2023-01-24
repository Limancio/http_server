#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "meow_hash_x64_aesni.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#if _WIN32

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#include <intrin.h>
#include <assert.h>

#include "basic/basic.h"
#include "basic/string.h"
#include "basic/basic_json.h"
#include "basic/basic_lexer.h"
#include "basic/thread.h"

#include "io/file_utils.h"
#include "io/source_file.h"

typedef SOCKET  net_socket;
typedef SOCKADDR net_socket_address;

#define close_socket(_socket) closesocket(_socket);
typedef HANDLE  semaphore_handle;

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <immintrin.h>

#include <dlfcn.h>
#include <pthread.h>

#include "basic/basic.h"
#include "basic/string.h"
#include "basic/basic_json.h"
#include "basic/basic_lexer.h"
#include "basic/thread.h"

#include "io/file_utils.h"
#include "io/source_file.h"

typedef s32 net_socket;
typedef sockaddr net_socket_address;

#define close_socket(_socket) close(_socket);
// typedef s32 semaphore_handle; // @incomplete

#endif

#include "http_server/database.h"

#define BACKLOG 20  // Passed to listen()
#define SOCKET_MAX_COUNT 100
#define MAX_GET_DATA     16
#define MAX_COOKIE_DATA  16

enum request_type {GET_REQUEST = 0, POST_REQUEST};

struct server_data;

// @NOTE: Start thread structures:
struct work_queue_entry {
    net_socket data; 
};

struct work_queue {
    volatile u32 completion_goal;
    volatile u32 completion_count;
    volatile u32 next_entry_to_write;
    volatile u32 next_entry_to_read;
    mutex_handle mutex_handle;
    
    work_queue_entry work_entries[256];
};

struct thread_update_info {
    u32 logical_index;
    server_data* server_data_pointer;
};

struct thread_work_info {
    u32 logical_index;
    work_queue *work_queue;
    server_data* server_data_pointer;
    
    string send_string;
    string recv_string;
    static_string string_url_path;
};
// End thread.

struct get_data {
    string key;
    string value;

    string file_name;
};

struct request_data {
    b32 api_call;
    static_string route_name;
    static_string route_file_name;
    static_string module_name;
    static_string internal_name;
    
    // @temporary: move to response_data struct?
    string extra_header_data;
    
    dll_handle module_handler;
    u32 array_count;	      
    get_data get_data_array[MAX_GET_DATA];
    
    get_data cookies_array[MAX_COOKIE_DATA];
    u32 cookies_array_count;	      
};

internal get_data
get_value_by_key(request_data* request_data, string key) {
    get_data result = {};
    
    for(u32 i = 0; i < request_data->array_count; i++) {
        get_data* it = &request_data->get_data_array[i];
        
        if(string_equals(it->key, key)) {
            result = *it;
            break;
        }
    }
    
    return(result);
}

struct fetch_value {
    element_list_arena element_list_arena;
    element root_element;
    string  fetch_string;
    
    string fetch_url;

    s32 elapsed_time;
    s32 refresh_time;
    
    b32 ready;
};

struct fetch_hash {
    s8 *key;
    fetch_value value;
};

struct server_data {
    work_queue work_queue;
    static_string string_url_path;
    
    thread_handle thread_array[16];
    u32 thread_count;

    database_data database_data;
    
    mutex_handle fetch_mutex_handle;
    u32 fetch_on_update;
    fetch_hash* fetch_map;
    string_bucket* fetch_string_list;
};

#include "http_server/constants.h"
#include "http_server/html_parser.cpp"
#include "http_server/http_request.cpp"

#endif // !HTTP_SERVER_H
