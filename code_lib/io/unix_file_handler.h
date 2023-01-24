#ifndef UNIX_FILE_HANDLER_H
#define UNIX_FILE_HANDLER_H

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

internal s32
unix_get_file_size(string path_name) {
    struct stat stat_data;
    stat(path_name.data, &stat_data);
    off_t size = stat_data.st_size;
    
    return((s32) size);
}

inline internal b32
unix_file_exists(string file_path) {
  return(access(file_path.data, F_OK) == 0);
}

internal b32
unix_read_file(string file_path, s8* buffer, size_t buffer_size) {
    b32 result;
    s32 file_descriptor;

    file_descriptor = open(file_path.data, O_RDONLY);
    if(file_descriptor < 0) {
        result = false;
    } else {
        size_t value = read(file_descriptor, buffer, buffer_size);
        result = (value != -1);

        s32 close_result = close(file_descriptor);
        // @TODO: error log.;
    }
    
    return(result);
}


#endif
