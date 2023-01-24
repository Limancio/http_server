#ifndef WIN32_FILE_HANDLER_H
#define WIN32_FILE_HANDLER_H

#include <windows.h>
#include <strsafe.h>

inline internal b32
file_type_equals(string str, string token);

internal LONGLONG
win32_get_file_size(string file_name) {
  HANDLE h_file;
  OVERLAPPED ol = {};
  
  h_file = CreateFileA(file_name.data,             // file to open
		       GENERIC_READ,          // open for reading
		       FILE_SHARE_WRITE | FILE_SHARE_READ, // share for reading
		       NULL,                  // default security
		       OPEN_EXISTING,         // existing file only
		       FILE_ATTRIBUTE_NORMAL, // normal file
		       NULL);                 // no attr. template

  LARGE_INTEGER file_size = {};

  if(h_file != INVALID_HANDLE_VALUE) {
    if(!GetFileSizeEx(h_file, &file_size)) {
      // CloseHandle(h_file);
    }
  }
  CloseHandle(h_file); // @temporary
  return(file_size.QuadPart);
}

internal b32
win32_delete_file(string file_name) {
  b32 result = DeleteFileA(file_name.data);
  return(result);
}

internal b32
win32_write_append_file(string file_name, u8* buffer, u32 buffer_size) {
  HANDLE h_file; 
  BOOL error_flag = FALSE;
  DWORD bytes_written = 0;
  
  h_file = CreateFileA(file_name.data,              // name of the write
		       FILE_APPEND_DATA,          // open for writing
		       FILE_SHARE_WRITE | FILE_SHARE_READ, // share for writing
		       NULL,                   // default security
		       OPEN_ALWAYS,          // create new file always
		       FILE_ATTRIBUTE_NORMAL,  // normal file
		       NULL);                  // no attr. template

  if(h_file != INVALID_HANDLE_VALUE) {
    error_flag = WriteFile(h_file,            // open file handle
			   buffer,            // start of data to write
			   buffer_size,	      // number of bytes to write
			   &bytes_written,    // number of bytes that were written
			   NULL); // no overlapped structure
  }
  
  CloseHandle(h_file);
  return((b32) error_flag);
}

internal b32
win32_write_append_file(string file_name, s8* buffer, u32 buffer_size) {
  HANDLE h_file; 
  BOOL error_flag = FALSE;
  DWORD bytes_written = 0;
  
  h_file = CreateFileA(file_name.data,              // name of the write
		       FILE_APPEND_DATA,          // open for writing
		       FILE_SHARE_WRITE | FILE_SHARE_READ, // share for writing
		       NULL,                   // default security
		       OPEN_ALWAYS,          // create new file always
		       FILE_ATTRIBUTE_NORMAL,  // normal file
		       NULL);                  // no attr. template

  if(h_file != INVALID_HANDLE_VALUE) {
    error_flag = WriteFile(h_file,            // open file handle
			   buffer,            // start of data to write
			   buffer_size,	      // number of bytes to write
			   &bytes_written,    // number of bytes that were written
			   NULL); // no overlapped structure
  }
  
  CloseHandle(h_file);
  return((b32) error_flag);
}

internal b32
win32_write_file(string file_name, s8* buffer, size_t buffer_size) {
  HANDLE h_file; 
  BOOL error_flag = FALSE;
  DWORD bytes_written = 0;
  
  h_file = CreateFileA(file_name.data,         // name of the write
		       GENERIC_WRITE,          // open for writing
		       FILE_SHARE_WRITE | FILE_SHARE_READ, // share for writing
		       NULL,                   // default security
		       CREATE_ALWAYS,          // create new file always
		       FILE_ATTRIBUTE_NORMAL,  // normal file
		       NULL);                  // no attr. template

  if(h_file != INVALID_HANDLE_VALUE) {
    error_flag = WriteFile(h_file,          // open file handle
			   buffer,          // start of data to write
			   (DWORD) buffer_size, // number of bytes to write
			   &bytes_written,  // number of bytes that were written
			   NULL);           // no overlapped structure
  }
  
  CloseHandle(h_file);
  return((b32) error_flag);
}

internal b32
win32_read_file(string file_path, s8* buffer, size_t buffer_size) {
  b32 result = true;
  
  HANDLE h_file;
  DWORD bytes_read;
  OVERLAPPED ol = {};
  
  h_file = CreateFileA(file_path.data,           // file to open
		     GENERIC_READ,          // open for reading
		     FILE_SHARE_WRITE | FILE_SHARE_READ,       // share for reading
		     NULL,                  // default security
		     OPEN_EXISTING,         // existing file only
		     FILE_ATTRIBUTE_NORMAL, // | FILE_FLAG_OVERLAPPED, // normal file
		     NULL);                 // no attr. template

  if(h_file != INVALID_HANDLE_VALUE) {
    b32 read = ReadFile(h_file, buffer, (DWORD) buffer_size, &bytes_read, &ol);
    
    // LPSTR messageBuffer = nullptr;
    // size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    //                           NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
  } else {
    result = false;
  }
  CloseHandle(h_file);

  return(result);
}

// @temporary:
internal b32
win32_read_file(string file_path, u8* buffer, size_t buffer_size) {
  b32 result = true;
  
  HANDLE h_file;
  DWORD bytes_read;
  OVERLAPPED ol = {};
  
  h_file = CreateFileA(file_path.data,           // file to open
		     GENERIC_READ,          // open for reading
		     FILE_SHARE_WRITE | FILE_SHARE_READ,       // share for reading
		     NULL,                  // default security
		     OPEN_EXISTING,         // existing file only
		     FILE_ATTRIBUTE_NORMAL, // | FILE_FLAG_OVERLAPPED, // normal file
		     NULL);                 // no attr. template

  if(h_file != INVALID_HANDLE_VALUE) {
    b32 read = ReadFile(h_file, buffer, (DWORD) buffer_size, &bytes_read, &ol);
  } else {
    result = false;
  }
  CloseHandle(h_file);

  return(result);
}

inline internal b32
win32_file_exists(string file_path) {
    DWORD dwAttrib = GetFileAttributes(file_path.data);
  
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

internal void
win32_read_file_name_array(string_bucket* bucket_list, string* file_name_array, string directory, string file_type = {}) {
  HANDLE handle = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATA ffd;
  LARGE_INTEGER file_size;
  
  u32 file_index = 0;
  handle = FindFirstFile(static_cast<TCHAR*>(directory.data), &ffd);
  if(handle != INVALID_HANDLE_VALUE) {
    do{
      if(ffd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE || ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
	file_size.LowPart  = ffd.nFileSizeLow;
        file_size.HighPart = ffd.nFileSizeHigh;
	s8* file_name = ffd.cFileName;
	
	size_t file_name_count = strlen(file_name);
	if((file_name_count == 1 && file_name[0] == '.') || (file_name_count == 2 && file_name[0] == '.' && file_name[1] == '.')) {
	  continue;
	}

	string temp_string = stack_string(file_name);
	if(string_empty(file_type) || (!string_empty(file_type) && file_type_equals(temp_string, file_type))) {
	  string_alloc(bucket_list, file_name_array[file_index], temp_string);
	  file_index += 1;
	}
      }
    } while(FindNextFile(handle, &ffd));
  }
  FindClose(handle);
}

internal u32
win32_get_file_count(string directory, string file_type = {}) {
  u32 file_count = 0;
  WIN32_FIND_DATA ffd;  
  HANDLE handle = INVALID_HANDLE_VALUE;
  
  handle = FindFirstFile((TCHAR*) directory.data, &ffd);
  if(handle != INVALID_HANDLE_VALUE) {
    do{
      if(ffd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE || ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
	s8* file_name = ffd.cFileName;
	size_t file_name_count = strlen(file_name);
	
	if((file_name_count == 1 && file_name[0] == '.') || (file_name_count == 2 && file_name[0] == '.' && file_name[1] == '.')) {
	  continue;
	}
	
	string string_file_name = cast_string(file_name, file_name_count, file_name_count);
	b32 ignore_file = false;
	if(!string_empty(file_type) && !file_type_equals(string_file_name, file_type)) {
	  ignore_file = true;
	}
	
	if(!ignore_file) {
	  file_count++;
	}
      }
    } while(FindNextFile(handle, &ffd));
  }
  
  FindClose(handle);
  return(file_count);
}

internal void
win32_get_current_directory(string& out) {
  memset(out.data, 0, out.data_size);
  GetCurrentDirectory(MAX_PATH, out.data);
  out.count = strlen(out.data);
}

#endif // !WIN32_FILE_HANDLER_H
