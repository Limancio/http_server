#ifndef SOURCE_FILE_H
#define SOURCE_FILE_H

#include "basic/string.h"

struct source_file {
  string file_name;
  string read_path;
  string write_path;
  
  s8* data_buffer;
  size_t data_size;
};

struct reader_file {
    b32 end_line;
    s8* eof;
    s8* parse_point;
};

internal void
init_reader(reader_file* reader, s8* buffer, size_t buffer_size) {
  reader->parse_point = &buffer[0];
  reader->eof = &buffer[buffer_size - 1];
}

internal void
get_next_line(string& line, reader_file* reader, b32 remove_empty_spaces = false) {
  line = {};
  
  s8*& p = reader->parse_point;
  s8* bol = p;
  b32 eol = false;
  
  size_t at = 0;
  while(!reader->end_line) {
    if((p[0] == '\r' && (p != reader->eof && p[1] == '\n')) ||
       (p[0] == '\n' && (p != reader->eof && p[1] == '\r'))) {
      p  += 2;
      at += 2;
      
      eol = true;
    } else if(p[0] == '\n' || p[0] == '\r') {
      p  += 1;
      at += 1;
      
      eol = true;
    } else {
      p  += 1;
      at += 1;
    }

    reader->end_line = ((p-1) == reader->eof);
    if(eol) { 
      line = {bol, at, at};
      reader->parse_point = p;
      break;
    }
  }
  
  if(!string_empty(line) && remove_empty_spaces) {
    while(line.data[0] == ' ') {
      line.data   = &line.data[1];
      line.count -= 1;
    }
    while(line.data[line.count - 1] <= 32) {
      line.count -= 1;
    }

  }
}

#endif // !SOURCE_FILE_H
