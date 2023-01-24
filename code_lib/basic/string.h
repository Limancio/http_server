#ifndef STRING_H
#define STRING_H

#include "basic/basic.h"
#include "basic/logger.h"

#define STRING_STACK_MAX 256

struct string {
    s8* data;
    
    size_t count;
    size_t data_size;
    
    b32 is_static;
};

struct string_link {
    s8* data_pointer;
    u32 use_count;
};

struct string_bucket {
    u8* data;
    size_t data_size;
    
    string_link* string_list;
    u32 list_count;
   
    string_bucket* next_bucket;
};

// @temporary: static_string function:
struct static_string {
    s8 data[STRING_STACK_MAX] = {};
    size_t count 	    = 0;
    size_t data_size  = STRING_STACK_MAX;
};

inline internal void
static_string_append(static_string& str, string append) {
    size_t empty_count     = str.data_size - str.count;
    size_t new_empty_count = empty_count - append.count; 
    append.count = (new_empty_count >= 0) ? append.count : (append.count - new_empty_count);
  
    memcpy(&str.data[str.count], append.data, append.count);
    str.count += append.count;
}

inline internal void
update_static_string(static_string& static_string, string string_data) {
    static_string.count = string_data.count;
    static_string.data_size = string_data.data_size;
}

#define static_to_string(_str) {_str.data, _str.count, _str.data_size}
#define string_exist(_str) (_str.data != NULL)

#define const_string_count(_string_data) sizeof(_string_data)-1
#define const_string(data) {(data), sizeof(data)-1, sizeof(data), true} // used for string literals
#define stack_string(data) {data, strlen(data), strlen(data)}           // used for string in stack 
#define cast_string(data, count, data_size) {data, count, data_size}    // used for s8* -> string casting

// @temporary: String without quotes
#define to_string(...) string{("" #__VA_ARGS__ ""), sizeof("" #__VA_ARGS__ "")-1, sizeof("" #__VA_ARGS__ ""), true}

//
// STRING_LIST:
//

#define STRING_BUCKET_CHAR_COUNT 64
#define STRING_LIST_COUNT        32

inline internal string_bucket*
get_new_bucket(size_t data_size, u32 link_count) {
    u32 string_link_count = (link_count < STRING_LIST_COUNT) ? STRING_LIST_COUNT : link_count;
  
    size_t string_bucket_size = sizeof(string_bucket);
    size_t string_link_size   = sizeof(string_link) * string_link_count;
  
    log_info("bucket_size [%zi], link_count: [%i], data_size: [%zi].\n", string_bucket_size, string_link_count, data_size);
  
    string_bucket* new_bucket = static_cast<string_bucket*>(virtual_alloc(string_bucket_size));
    new_bucket->data_size     = string_link_count * STRING_BUCKET_CHAR_COUNT; // data_size;
    new_bucket->data	      = static_cast<u8*>(virtual_alloc(new_bucket->data_size));
    
    new_bucket->string_list   = static_cast<string_link*>(virtual_alloc(string_link_size));
    new_bucket->list_count    = string_link_count;
  
    memset(new_bucket->data, 0, sizeof(s8) * new_bucket->data_size); // @temporary: out.data_size
    return(new_bucket);
}

internal void
init_string_data(string& out_string, u8* char_data, string_link* link, u32 link_count) {
    link->data_pointer = reinterpret_cast<s8*>(char_data); 
    link->use_count    = link_count;	
  
    out_string.data      = link->data_pointer;
    out_string.count     = 0;
    out_string.data_size = link_count * STRING_BUCKET_CHAR_COUNT;
}

internal void
get_string_from_bucket(string* out, string_bucket*& bucket_list, size_t data_size) {
    string result = {};
    u32 target_link_count = (u32) floor((r32) data_size / STRING_BUCKET_CHAR_COUNT) + 1;
  
    if(bucket_list == NULL) {
        bucket_list = get_new_bucket(data_size, target_link_count);
    }
  
    string_bucket* it = bucket_list;
    while(it && !string_exist(result)) {
        u32 counter = 0;
        u32 start_index = 0;
    
        u32 i = 0;
        for(;;) {
            string_link* link = &it->string_list[i];

            if(i < it->list_count) {
                if(link->use_count) {
                    if(counter != 0) counter = 0;
                    i += link->use_count;
                    start_index = i;
                } else {
                    counter += 1;
                    i += 1;
                }
	
                if(counter == target_link_count) {
                    init_string_data(result, &it->data[start_index * STRING_BUCKET_CHAR_COUNT], &it->string_list[start_index], target_link_count);
                    break;
                }
            } else {
                break;
            }                     
        }
    
        if(!string_exist(result) && !it->next_bucket) {
            it->next_bucket = get_new_bucket(data_size, target_link_count);
        }
    
        it = it->next_bucket;
    }
  
    *out = result;
}

internal void
free_string_arena(string_bucket* bucket_list) {
    while(bucket_list) {
        string_bucket* next = bucket_list->next_bucket;
    
        virtual_free(bucket_list->data, bucket_list->data_size);
        virtual_free(bucket_list->string_list, sizeof(string_link) * bucket_list->list_count);
        virtual_free(bucket_list, sizeof(string_bucket));
        bucket_list = NULL;
    
        bucket_list = next;
    }
}

// @incomplete:
inline internal void
string_free(string_bucket*& bucket_list, string& str) {
    string_bucket* it = bucket_list;
    while(it) {
    
        u32 index = 0;
        for(;;) {
            if(index >= it->list_count) {
                break;
            }
      
            string_link* link = &it->string_list[index];
      
            if(str.data == link->data_pointer) {
                memset(link->data_pointer, 0, link->use_count * STRING_BUCKET_CHAR_COUNT);
                link->use_count = 0;
	
                str = {};
                return;
            } else {
                index += (link->use_count) ? link->use_count : 1;
            }
        }
    
        it = it->next_bucket;
    }
}

//
// STRING UTILITY FUNCTIONS:
//

inline internal b32
string_contents(string& str) {
    b32 result = false;
    for(u32 i = 0; i < str.count; i++) {
        if(str.data[i] > 32) {
            result = true;
            break;
        }
    }

    return(result);
}

inline internal void
string_clear_data(string& str) {
    memset(str.data, 0, str.data_size);
    str.count = 0;
}

inline internal b32
string_empty(const string& str) {
    b32 is_empty = false;
  
    if(str.data == NULL || str.count == 0) {
        is_empty = true;
    }
  
    return(is_empty);
}

inline internal string
string_append(string& str, s8* append_data, size_t append_count) {
    size_t empty_count     = str.data_size - str.count;
    size_t new_empty_count = empty_count - append_count; 
    append_count = (new_empty_count >= 0) ? append_count : (append_count - new_empty_count);
  
    memcpy(&str.data[str.count], append_data, append_count);
    str.count += append_count;
  
    return(str);
}

inline internal string
string_append(string& str, string append_string) {
    return(string_append(str, append_string.data, append_string.count));
}

inline internal string
string_append(string& str, s8 value) {
    return(string_append(str, &value, 1));
}

inline internal b32
string_equals(const string& str, const string& token) {
    return(str.count == token.count && memcmp(str.data, token.data, token.count) == 0);
}

inline internal void
string_delete_at(string& str, u32 at, u32 delete_count) {
    if(at < str.count) {
        str.data = &str.data[at + delete_count];
        str.count -= delete_count;
    }
}

inline internal void
string_append_at(string& str, u32 at, string append) {
    if((str.count + append.count) < str.data_size) {
        memcpy(&str.data[at + append.count], &str.data[at], sizeof(s8) * (str.count + append.count));
        memcpy(&str.data[at], append.data, sizeof(s8) * append.count);
    } else {
        // @incomplete: Log error.
    }
}

inline internal void
string_replace_at(string& str, u32 replace_start, u32 replace_end, string replace_string) {
    // @incomplete: Log error.
    size_t remain_data_size = str.count - replace_end; 
    
    memcpy(&str.data[replace_start + replace_string.count], &str.data[replace_end], sizeof(s8) * remain_data_size);
    memcpy(&str.data[replace_start], replace_string.data, sizeof(s8) * replace_string.count);
    
    size_t replace_size = replace_end - replace_start;
    str.count -= replace_size;
    str.count += replace_string.count;
}

internal b32
char_is_empty(s8 x) {
    return(x == ' ' || x == '\t' || x == '\r' || x == '\n' || x == '\f');
}

inline internal void
string_clear_empty(string& string);

// @note: substring cannot be free.
inline internal string
string_substring(const string& str, size_t count) {
    string result = cast_string(str.data, count, str.data_size);
    return(result);
}

inline internal string
string_substring_at(const string& str, s8* string_data_at) {
    string result = {};

    size_t at = (reinterpret_cast<size_t>(string_data_at)) - (reinterpret_cast<size_t>(str.data));
    if(at < str.data_size) {
        result = cast_string(&str.data[at], str.count - at, str.data_size);
    }

    // @temporary:
    string_clear_empty(result);
  
    return(result);
}

inline internal string
string_substring_at(const string& str, size_t at) {
    string result = {};

    if(at < str.data_size) {
        result = cast_string(&str.data[at], str.count - at, str.data_size);
    }

    // @temporary:
    string_clear_empty(result);
  
    return(result);
}

// @temporary:
inline internal void
string_clear_empty(string& string) {
    while(!string_empty(string) && char_is_empty(string.data[0])) { 
        string = string_substring_at(string, 1);
    }
  
    while(!string_empty(string) && char_is_empty(string.data[string.count-1])) { 
        string.count -= 1;
    }
}

inline internal void
string_remove_quotes(string& string) {
    while(!string_empty(string) && string.data[0] == '\"') { 
        string = string_substring_at(string, 1);
    }
  
    while(!string_empty(string) && string.data[string.count - 1] == '\"') { 
        string.count -= 1;
    }
}

inline internal b32
string_starts_with(const string& str, const string& token) {
    if(str.count < token.count) {
        return(false);
    }
  
    string sub_string = string_substring(str, token.count);
    return(string_equals(sub_string, token));
}

inline internal b32
string_contains(const string& str, const string& token) {
    if(str.count < token.count) {
        return(false);
    }
  
    for(u32 i = 0; i <= str.count - token.count; i++) {
        string sub_string = cast_string(&str.data[i], token.count, token.count);
    
        if(string_equals(sub_string, token)) {
            return(true);
        }
    }
  
    return(false);
}

inline internal string
string_token(const string& str, s8 token) {
    string sub_string = {};
  
    for(u32 count = 0; count < str.count; count++) {
        s8 c = str.data[count];
    
        if(c == token) {
            sub_string = string_substring(str, count);
            break;
        }
    }
  
    return(sub_string);
}

inline internal string
string_get_substring(string in, s8 token, b32 do_clear_empty = false) { // @incomplete: Replace s8 to string
    string result = {};
    static string cache_in = {};
  
    string read_string;
    if(string_empty(in)) {
        if(string_empty(cache_in)) {
            return(result);
        } else {
            read_string = cache_in;
        }
    } else {
        cache_in    = in;
        read_string = in;
    }

    if(do_clear_empty) {
        string_clear_empty(read_string);
    }
    
    for(u32 i = 0; i < read_string.count; i++) {
        s8 c = read_string.data[i];

        if(c == token) {
            result = string_substring(read_string, i);
            break;
        }
    }
  
    if(string_empty(result)) {
        cache_in = {};
        result = read_string;
    } else {
        // @note: add +1 for the token.
        cache_in.data   = &result.data[result.count + 1];
        cache_in.count -= result.count + 1;
    }
  
    return(result);
}

inline internal u32
string_word_count(const string& str) {
    u32 count = 0;
    b32 do_count = true;
    for(u32 i = 0; i < str.count; i++) {
        s8 c = str.data[i];
    
        if(char_is_empty(c)) {
            if(do_count) {
                count += 1;
                do_count = false;
            }
        } else {
            do_count = true;
        }
    }

    if(count > 0) {
        count += 1;
    }

    return(count);
}

// @incomplete:
#if 0
inline internal void
get_string_by(string& out, const string& str, const s8& token) {
    string result = {};
  
    b32 asign = false;
    u32 start_index = 0;
  
    for(u32 i = 0; i < str.count; i++) {
        if(string_at(str, i) == token) {
            start_index = i + 1;
            asign = true;
            break;
        }
    }

    if(asign) {
        result.data  = &str.data[start_index];
        result.count = (str.count - start_index); 
        result.data_size = result.count;
    }

    out = result;
}
#endif

//
// STRING CREATION FUNCTIONS:
//

inline internal void
string_alloc(string_bucket*& bucket_list, string& str, size_t data_size = STRING_BUCKET_CHAR_COUNT) {
    get_string_from_bucket(&str, bucket_list, data_size);
}

inline internal void
string_alloc(string_bucket*& bucket_list, string& str, string copy_string) {
    get_string_from_bucket(&str, bucket_list, copy_string.count);
    string_append(str, copy_string);
}

inline internal void
string_alloc(string_bucket*& bucket_list, string& str, s8* char_array) {
    size_t char_count = strlen(char_array);
  
    get_string_from_bucket(&str, bucket_list, char_count);
    string_append(str, char_array, char_count);
}

inline internal void
string_alloc(string_bucket*& bucket_list, string& str, s8* char_array, size_t char_count) {
    get_string_from_bucket(&str, bucket_list, char_count);
    string_append(str, char_array, char_count);
}

inline internal void
concatenate(string_bucket*& bucket_list, string& str, s32 n, ...) {
    va_list args;
    va_start(args, n);

    size_t total_count = 0;
    for(s32 i = 0; i < n; i++) {
        s8* str = va_arg(args, s8*);
        total_count += strlen(str);
    }

    // @temporary:
    va_end(args);
    va_start(args, n);
  
    string_alloc(bucket_list, str, total_count);
    for(s32 i = 0; i < n; i++) {
        s8* append = va_arg(args, s8*);
        string_append(str, append, strlen(append));
    }
  
    va_end(args);
}

inline internal void
string_concatenate(string_bucket*& bucket_list, string& str, s32 n, ...) {
    va_list args;
    va_start(args, n);
  
    size_t total_count = 0;
    for(s32 i = 0; i < n; i++) {
        string str = va_arg(args, string);
        total_count += str.count;
    }

    // @temporary:
    va_end(args);
    va_start(args, n);
  
    string_alloc(bucket_list, str, total_count);
    for(s32 i = 0; i < n; i++) {
        string append = va_arg(args, string);
        string_append(str, append);
    }

    va_end(args);
}

inline internal void
string_replace(string_bucket* bucket_list, string& out, const string& in, const string& token, const string& value) {
    string result;
  
    size_t new_count = 0;
    size_t at = 0;
    while(at < in.count) {
        string find_string = cast_string(&in.data[at], token.count, token.count);
        if(string_equals(find_string, token)) {
            at += token.count;
            new_count += value.count;
        } else {
            at += 1;
            new_count += 1;
        }
    }
    
    string_alloc(bucket_list, result, new_count);
    string_append(result, in);
    
    size_t read_at  = 0;
    size_t write_at = 0;
    for(;;) {
        b32 parse_char = true;
    
        if(read_at < in.count) {
            string find_string = cast_string(&in.data[read_at], token.count, token.count);
      
            if(string_equals(find_string, token)) {
                parse_char = false;
	
                memcpy(&result.data[write_at], value.data, value.count);
                write_at += value.count;
                read_at  += find_string.count;
            }
        } else {
            break;
        }
    
        if(parse_char) {
            result.data[write_at] = in.data[read_at];
            write_at += 1;
            read_at  += 1;
        }
    }
    result.count = new_count;
  
    if(out.data_size < new_count || out.is_static) {
        // string_free(bucket_list, out);    @incomplete: out string cannot be free. because we dont know if is a allocated string.
        out = result;
    } else {
        memset(out.data, 0, sizeof(s8) * out.data_size); // @temporary: out.data_size
        memcpy(out.data, result.data, sizeof(s8) * result.count);
        out.count = result.count;
        
        string_free(bucket_list, result);
    }
}

//
// @incomplete: to_string functions
//

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

inline internal u32
string_to_u32(string string_value) {
    return(static_cast<u32>(strtoul(string_value.data, 0, 10)));
}

inline internal s32
string_to_s32(string string_value) {
    return(static_cast<s32>(atoi(string_value.data)));
}

inline internal r32
string_to_r32(string string_value) {
    return(static_cast<r32>(atof(string_value.data)));
}

inline internal r32
string_byte_to_r32(string string_value) {
    r32u r32_data;

    s32 hex_data;
    hex_data = static_cast<s32>(strtoul(&string_value.data[1], 0, 16));
  
    r32_data.bytes[0] = (hex_data >> 24) & 0xFF;
    r32_data.bytes[1] = (hex_data >> 16) & 0xFF;
    r32_data.bytes[2] = (hex_data >> 8)  & 0xFF;
    r32_data.bytes[3] = hex_data & 0xFF;
  
    return(r32_data.value);
}

inline internal void
u32_to_string(string& out, u32 value) {
    memset(out.data, 0, out.data_size);
    stbsp_sprintf(out.data, "%u", value);
    out.count = strlen(out.data);
}

inline internal void
s32_to_string(string& out, s32 value) {
    memset(out.data, 0, out.data_size);
    stbsp_sprintf(out.data, "%i", value);
    out.count = strlen(out.data);
}

inline internal void
r32_to_string(string& out, r32 value) {
    memset(out.data, 0, out.data_size);
    stbsp_sprintf(out.data, "%f", value);
    out.count = strlen(out.data);
}

inline internal void
b32_to_string(string& out, b32 value) {
    const s8* result = (value) ? "true" : "false";
    stbsp_sprintf(out.data, "%s", value);
    out.count = strlen(out.data);
}

// @temporary:
const string directory_open_token  = const_string("\\*");
const string directory_slash_token = const_string("\\");
const string directory_back_token  = const_string("\\..");
const string null_terminate_token  = const_string("\0");
const string end_line_token	   = const_string("\n");

#endif // !STRING_H
