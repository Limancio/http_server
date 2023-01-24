#ifndef MEMORY_H
#define MEMORY_H

#include "logger.h"

#if _WIN32
#include <windows.h>

inline internal void*
win32_virtual_alloc(u64 size) {
    void* result = VirtualAlloc(0, size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    if(!result) {
        log_error("Alloc Failed.\n");
        ExitProcess(1);
    }
    return(result);
}

inline internal void
win32_virtual_free(void* pointer, u64 size) {
    u32 result = VirtualFree(pointer, 0, /*MEM_DECOMMIT|*/MEM_RELEASE);
}

#define virtual_alloc(_size) win32_virtual_alloc(_size)
#define virtual_free(_pointer, _size) win32_virtual_free(_pointer, _size)
#else
#include <sys/mman.h>

inline internal void*
unix_virtual_alloc(size_t size) {
    void* result = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    // log_info("virtual_alloc [%zi].\n", size);
    
    if(result == MAP_FAILED) {
        log_error("Mapping Failed.\n");
        exit(1);
    }
    return(result);
}

inline internal void
unix_virtual_free(void* pointer, size_t size) {
    s32 result = munmap(pointer, size);
    // log_info("virtual_free [%zi].\n", size);
    if(result == -1) {
        log_error("munmap Failed.\n");
        exit(1);
    }
}

#define virtual_alloc(_size) unix_virtual_alloc(_size)
#define virtual_free(_pointer, _size) unix_virtual_free(_pointer, _size)
#endif

#define create_arena(value_name, struct_name, array_size)               \
    struct struct_name##_bucket {                                       \
        value_name struct_name##_array[array_size];                     \
        u32 array_count;                                                \
        struct_name##_bucket* next_bucket;                              \
    };                                                                  \
                                                                        \
    struct struct_name##_arena {                                        \
        struct_name##_bucket* bucket_list;                              \
        u32 bucket_count;                                               \
    };                                                                  \
                                                                        \
    inline internal struct_name##_bucket*                               \
    create_##struct_name##_bucket(struct_name##_arena* arena) { 		\
        struct_name##_bucket* new_bucket = static_cast<struct_name##_bucket*>(virtual_alloc(sizeof(struct_name##_bucket))); \
        arena->bucket_count += 1;                                       \
        return(new_bucket);                                             \
    }                                                                   \
    internal value_name*                                                \
    create_##struct_name( struct_name##_arena* arena) {                 \
        value_name* result = 0;                                         \
        if(arena->bucket_count == 0) {                                  \
            arena->bucket_list = create_##struct_name##_bucket(arena);  \
        }                                                               \
                                                                        \
        struct_name##_bucket* bucket = arena->bucket_list;              \
        while(bucket) {                                                 \
            if(bucket->array_count < array_size) {                      \
                u32 index = bucket->array_count;                        \
                bucket->array_count += 1;                               \
                result = &bucket->struct_name##_array[index];	  		\
                *result = {};                                           \
                break;                                                  \
            } else if(bucket->next_bucket == NULL){                     \
                bucket->next_bucket = create_##struct_name##_bucket(arena); \
            }                                                           \
                                                                        \
            bucket = bucket->next_bucket;                               \
        }                                                               \
                                                                        \
        return(result);                                                 \
                                                      }                 \
                                                                        \
                                                      internal void     \
                                                      clear_##struct_name( struct_name##_bucket* bucket) { \
                                                          if(bucket) {  \
                                                              clear_##struct_name (bucket->next_bucket); \
                                                              virtual_free(bucket, sizeof(struct_name##_bucket)); \
                                                          }             \
                                                      }				

#define create_list(struct_name, list_name, value_name, array_count)	\
    struct struct_name##_list {                                         \
        struct_name struct_name;                                        \
        struct_name##_list* next;                                       \
    };                                                                  \
                                                                        \
    create_arena(struct_name##_list, struct_name##_list, array_count);	\
                                                                        \
    internal struct_name*                                               \
    get_child_at(struct_name##_list##_arena* arena, value_name* value, u32 index) { \
        struct_name* child;                                             \
                                                                        \
        if(value->list_name == NULL) {                                  \
            value->list_name = create_##struct_name##_list(arena);      \
            value->list_name->next = 0;                                 \
        }                                                               \
                                                                        \
        struct_name##_list* child_list = value->list_name;          	\
        struct_name##_list* parent_list = 0;                            \
        u32 child_count = 0;                                            \
        for(;;) {                                                       \
            if(child_list == NULL) {                                    \
                child_list = create_##struct_name##_list(arena);		\
                if(parent_list) {                                       \
                    parent_list->next = child_list;                     \
                }                                                       \
            }                                                           \
                                                                        \
            if(child_count == index) {                                  \
                child = &child_list->struct_name;                       \
                *child = {};                                            \
                break;                                                  \
            }                                                           \
                                                                        \
            child_count++;                                              \
            parent_list = child_list;                                   \
            child_list  = child_list->next;                             \
        }                                                               \
                                                                        \
        return(child);                                                  \
    }                                                                   \
                                                                        \
    internal void                                                       \
    delete_child_at(struct_name##_list##_arena* arena, value_name* value, u32 index) { \
        struct_name##_list* child_list = value->list_name;              \
        if(index == 0) {                                                \
            *child_list = {};                                           \
        } else {                                                        \
            u32 i = 0;                                                  \
                                                                        \
            for(;;) {                                                   \
                if((i+1) == index) {                                    \
                    struct_name##_list* to_delete = child_list->next;   \
                    child_list->next = to_delete->next;                 \
                    *to_delete = {};                                    \
                    break;                                              \
                }                                                       \
                child_list = child_list->next;                          \
                i += 1;                                                 \
            }                                                           \
        }                                                               \
    }									

#endif
