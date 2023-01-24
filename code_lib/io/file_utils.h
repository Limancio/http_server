#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#if _WIN32
#include "io/win32_file_handler.h"

#define get_file_size(file_path) win32_get_file_size(file_path)
#define write_file(file_path, buffer, buffer_size) win32_write_file(file_path, buffer, buffer_size)
#define write_append_file(file_path, buffer, buffer_size) win32_write_append_file(file_path, buffer, buffer_size)
#define delete_file(file_path) win32_delete_file(file_path)
#define read_file(file_path, buffer, buffer_size) win32_read_file(file_path, buffer, buffer_size)
#define read_file_name_array(...) win32_read_file_name_array(__VA_ARGS__)
#define get_file_count(directory, ...) win32_get_file_count(directory, __VA_ARGS__)
#define get_current_directory(string) win32_get_current_directory(string)
#define file_exists(file_path) win32_file_exists(file_path)
#else
#include "io/unix_file_handler.h"

#define get_file_size(file_name) unix_get_file_size(file_name)
#define write_file(file_path, buffer, buffer_size) 
#define write_append_file(file_path, buffer, buffer_size)
#define delete_file(file_path) 
#define read_file(file_path, buffer, buffer_size) unix_read_file(file_path, buffer, buffer_size)
#define read_file_name_array(...) 
#define get_file_count_from_directory(directory, ...) 
#define get_current_directory(string_arena) 
#define file_exists(file_path) unix_file_exists(file_path) 
#endif  

// @incomplete: Move to other file.
inline internal b32
file_type_equals(string str, string token) {
  b32 is_equals = false;
  
  if(str.count > token.count) {
    for(u32 i = 0; ; i++) {
      if(i < token.count) {
	if(str.data[str.count - i] != token.data[token.count - i]) {
	  break;
	}
      } else {
	is_equals = true;
	break;
      }
    }
  }
  
  return(is_equals);
}

inline internal void
get_file_name(string* out, string in) {
  u32 at = 0;
  for(u32 i = 0; i < in.count; i++) {
    if(in.data[in.count - i] == '/' || in.data[in.count - i] == '\\') {
      at = i;
      break;
    }
  }
  out->data  = &in.data[in.count - at + 1];
  out->count = at - 1;
  out->data_size = out->count;
}

// @incomplete: Error message.
internal void
load_file(temporary_pool* memory_pool, string file_path, s8** out_buffer, size_t* out_size) {
  size_t buffer_size = get_file_size(file_path);
  s8* buffer = static_cast<s8*>(temporary_alloc(memory_pool, buffer_size + 1));
  
  read_file(file_path, buffer, buffer_size);
  buffer[buffer_size] = '\n';
  
  *out_buffer = buffer;
  *out_size  = buffer_size;
}
#if 0
internal void
parse_lines(line_arena* line_arena, s8* buffer, size_t buffer_size, b32 ignore_empty_line = false) {
  size_t at = 0;
  u32 bol = 0;
  u32 line_count = 0;
  for(; at < buffer_size; ) {
    s8 c0 = buffer[at];
    s8 c1 = buffer[at+1];
    b32 hit_eol = false;
    u32 eol = at;
    
    if((c0 == '\r' && c1 == '\n') ||
       (c0 == '\n' && c1 == '\r')) {
      hit_eol = true;
      at += 2;
    } else if(c0 == '\r') {
      hit_eol = true;
      at += 1;
    } else if(c0 == '\n') {
      hit_eol = true;
      at += 1;
    } else {
      at += 1;
    }

    if(hit_eol) {
      if(ignore_empty_line == false || (bol < eol)) {
	string* line = create_line(line_arena);
	line_count++;
	
#if 0	
	while(buffer[bol] == ' ') { // @temporary: clearing empty space in bol
	  bol++;
	}
#endif
	
	line->count = eol - bol;
	line->data  = buffer + bol;
      }
      
      bol = at;
    }
  }
}
#endif

// @incomplete:
#if 0
internal void
delete_all_files_from_directory(string_arena* arena, string level_directory) {
  string string_level_directory = concatenate(arena, 3, LEVEL_PATH, level_directory.data, "/*");
  
  u32 files_count = get_file_count_from_directory(string_level_directory);
  string* file_path_array  = (string*) malloc(sizeof(string) * files_count);
  
  memset(file_path_array, 0, sizeof(string) * files_count);
  read_all_files_from_directory(arena, file_path_array, string_level_directory);
  for(u32 i = 0; i < files_count; i++) {
    string string_file_name = concatenate(arena, 4, LEVEL_PATH, level_directory.data, "/", file_path_array[i].data);
    
    delete_file(string_file_name);
    
    free_string(arena, &string_file_name);
  }
  
  free(file_path_array);
  free_string(arena, &string_level_directory);
}
#endif

#endif // !FILE_UTILS_H
