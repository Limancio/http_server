#ifndef BASIC_LEXER_H
#define BASIC_LEXER_H

#include "io/source_file.h"

enum token_type {
    LEX_ID = 256,
    LEX_EOF,
    LEX_PARSE_ERROR,
    LEX_STRUCT_CLOSE,
    LEX_NOTE,
    LEX_STRING,
    LEX_EQUAL
};

#define PARSE_STRING     0x1
#define PARSE_URL_VALUES 0x2

struct lex_location {
    s32 line_number;
    s32 line_offset;
};

struct lexer {
    s32 token;
    string string_token;
    u32 parse_flags;
  
    reader_file reader;
};

internal void
lexer_init(lexer* lexer, s8* parse_point, s8* eof, u32 parse_flags = 0) {
    lexer->reader.parse_point = parse_point;
    lexer->reader.eof = eof;
    lexer->parse_flags = parse_flags;
}

internal s32
lexer_token(lexer* lexer, s32 token, s8* start, u32 count, u32 offset) {
    lexer->token = token;
    lexer->string_token.data  = start;
    lexer->string_token.count = count;
    lexer->string_token.data_size = count;
  
    lexer->reader.parse_point  = start + offset;
  
    return(1);
}

internal s32 lexer_eof(lexer* lexer) {
    lexer->token = LEX_EOF;
    return(0);
}

internal s32
lexer_is_empty(int x) {
    return(x == ' ' || x == '\t' || x == '\r' || x == '\n' || x == '\f');
}

internal s32
lexer_get_token(lexer* lexer) {
    reader_file& reader = lexer->reader;
    s8*& p = reader.parse_point;

    // @temporary:
    lexer->string_token.data  = 0;
    lexer->string_token.count = 0;
    
    for(;;) {
        while(p != reader.eof && lexer_is_empty(*p)) {
            p++;
        }
    
        if(p != reader.eof && p[0] == '/' && p[1] == '/') {
            while (p != reader.eof && *p != '\r' && *p != '\n') {
                p++;
            }
            continue;
        }
    
        if(p != reader.eof && p[0] == '/' && p[1] == '*') {
            p += 2;
      
            while(p != reader.eof && (p[0] != '*' || p[1] != '/')) {
                p++;
            }
            if(p != reader.eof) {
                p += 2;
            }
            continue;
        }

#if 0
        if(p != reader.eof && p[0] == '#') {
            while(p != reader.eof && *p != '\r' && *p != '\n') {
                ++p;
            }
            continue;
        }
#endif    
        break;
    }
  
    if(p == reader.eof) {
        return(lexer_eof(lexer));
    }
    
    if(lexer->parse_flags & PARSE_URL_VALUES) {
        if(*p > 32) {
            s32 n = 0;
      
            do {
                n += 1;
            } while(p[n] != '/' && p[n] > 32);
                    
            return(lexer_token(lexer, LEX_ID, p, n, n));
        }    
    }

    switch(*p) {
        default: {
            if((*p >= 'a' && *p <= 'z')
               || (*p >= 'A' && *p <= 'Z')
               || *p == '_' || (u8) *p >= 128) {
                s32 n = 0;
                
                do {
                    n += 1;
                } while((p[n] >= 'a' && p[n] <= 'z')
                        || (p[n] >= 'A' && p[n] <= 'Z')
                        || (p[n] >= '0' && p[n] <= '9') // allow digits in middle of identifier
                        || p[n] == '-' || p[n] == '_' || (u8) p[n] >= 128);
                
                return(lexer_token(lexer, LEX_ID, p, n, n));
            }    
            
    single_char:
            return(lexer_token(lexer, *p, p, 1, 1));
        }
            // @incomplete: 
        case '{': {
            goto single_char;
        }
            // @incomplete: 
        case '}': {
            if(p != reader.eof && p[1] == ';') return(lexer_token(lexer, LEX_STRUCT_CLOSE, p, 2, 2)); // @incomplete: used for struct close, can be: int bar = {1.0f};
            else goto single_char;
        }
        case '@': {
            s32 n = 0;
    
            do {
                n += 1;
            } while((p[n] >= 'a' && p[n] <= 'z')
                    || (p[n] >= 'A' && p[n] <= 'Z')
                    || (p[n] >= '0' && p[n] <= '9') // allow digits in middle of identifier
                    || p[n] == '_' || (u8) p[n] >= 128);
    
            return(lexer_token(lexer, LEX_NOTE, p, n, n));
        }
        case '+': {
            goto single_char;
        }
        case '-': {
            goto single_char;
        }
        case '&': {
            goto single_char;
        }
        case '|': {
            goto single_char;
        }
        case '=': {
            goto single_char;
        }
        case '!': {
            goto single_char;
        }
        case '^': {
            goto single_char;
        }
        case '%': {
            goto single_char;
        }
        case '*': {
            goto single_char;
        }
        case '/': {
            goto single_char;
        }
        case '<': {
            goto single_char;
        }
        case '>': {
            goto single_char;
        }
        case '"': {
            if(lexer->parse_flags & PARSE_STRING) {
                s32 n = 0;
    
                do {
                    n += 1;
                } while(p[n] != '"'
                        || (p[n] >= 'a' && p[n] <= 'z')
                        || (p[n] >= 'A' && p[n] <= 'Z')
                        || (p[n] >= '0' && p[n] <= '9') // allow digits in middle of identifier
                        || p[n] == '_' || (u8) p[n] >= 128);
    
                return(lexer_token(lexer, LEX_STRING, p+1, n-1, n));
            } else {
                goto single_char;
            }
        }
        case '\'': {
            goto single_char;
        }
        case '0': {
            goto single_char;
        }
            /*case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
              goto single_char;
              }*/
    }
  
    return(0);
}

#endif // !BASIC_LEXER_H
