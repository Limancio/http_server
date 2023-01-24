#ifndef BASIC_H
#define BASIC_H

#include <stdlib.h>
#include "types.h"
#include "memory.h"

#ifdef _WIN32

#  define read_barrier()  _ReadBarrier()
#  define write_barrier() _WriteBarrier()

#  define MODULE_EXPORT __declspec(dllexport)
   typedef HMODULE dll_handle;

#  define get_procedure_address(_handler, _prodecure_name) GetProcAddress(_handler, _prodecure_name)
#  define load_library(_library_path) LoadLibraryA(_library_path)
#  define free_library(_handler) FreeLibrary(_handler)

#else

#  define read_barrier()  __asm__ __volatile__("":::"memory")
#  define write_barrier() __asm__ __volatile__("":::"memory")

#  define MODULE_EXPORT extern
   typedef void* dll_handle;

#  define get_procedure_address(_handler, _prodecure_name) dlsym(_handler, _prodecure_name)
#  define load_library(_library_path) dlopen(_library_path, RTLD_LAZY)
#  define free_library(_handler) dlclose(_handler)
#endif



inline internal uintptr_t
pointer_to_int(void* address) {
  return(reinterpret_cast<uintptr_t>(address));
}

struct dormammu_buffer {
  s8* buffer;
  size_t cursor_index;
  
  size_t empty_size;
  size_t buffer_size;
};

inline internal void
set_buffer_index(dormammu_buffer* dormammu, size_t new_index) {
  dormammu->cursor_index = new_index; // @incomplete
}

inline internal size_t
get_buffer_size(const dormammu_buffer* dormammu) {
  return(dormammu->cursor_index); //return(dormammu->buffer_size - dormammu->empty_size);
}

inline internal void
copy_value_at(dormammu_buffer* dormammu, void* out, size_t value_size, size_t& index) {
  if(index >= 0 && index < dormammu->buffer_size) {
    memcpy(out, &dormammu->buffer[index], value_size);
    index += value_size;
  }
}

inline internal void
clear_dormammu(dormammu_buffer* dormammu) {
  dormammu->empty_size   = 0;
  dormammu->cursor_index = 0;
    
  memset(dormammu->buffer, 0, dormammu->buffer_size);
}

inline internal void
feed_dormammu(dormammu_buffer* dormammu, void* data, size_t size) {
  if(dormammu->buffer == NULL) {
    dormammu->buffer_size = ((size / 4096) + 1) * 4096;
    dormammu->buffer = static_cast<s8*>(virtual_alloc(dormammu->buffer_size));
    // dormammu->empty_size  = dormammu->buffer_max_size;
  }
  
  size_t empty_size = dormammu->buffer_size - dormammu->cursor_index;
  
  if(empty_size <= size) {
    size_t old_buffer_size = dormammu->buffer_size;
    size_t new_buffer_size = old_buffer_size + ((size / 4096) + 1) * 4096;
    s8* new_buffer = static_cast<s8*>(virtual_alloc(new_buffer_size));
    
    memcpy(new_buffer, dormammu->buffer, old_buffer_size);
    virtual_free(dormammu->buffer, old_buffer_size);
    
    dormammu->buffer 	  = new_buffer;
    dormammu->buffer_size = new_buffer_size;
    // dormammu->empty_size  = (new_buffer_size + dormammu->empty_size) - old_buffer_size;
  }
  
  void* buffer_pointer = &dormammu->buffer[dormammu->cursor_index];
  dormammu->cursor_index += size;
  
  // u32 index = (u32) (dormammu->buffer_size - dormammu->empty_size);
  // dormammu->empty_size -= size;
  
  if(data == NULL) memset(buffer_pointer, 0, size);
  else 		   memcpy(buffer_pointer, data, size);
}

inline internal void
feed_dormammu(dormammu_buffer* out, dormammu_buffer* in) {
  size_t buffer_size = get_buffer_size(in);
  feed_dormammu(out, in->buffer, buffer_size);
}

struct temporary_pool {
  u8* buffer;
  size_t empty_size;
  size_t buffer_size;

  temporary_pool* next;
};

internal void*
temporary_alloc(temporary_pool*& pool, size_t size) {
  void* buffer = 0;
  
  if(pool == NULL) {
    pool = static_cast<temporary_pool*>(malloc(sizeof(temporary_pool))); // @temporary: replace for dormammu alloc
    memset(pool, 0, sizeof(temporary_pool));
    
    pool->buffer_size = ((size / 4096) + 1) * 4096;
    pool->empty_size  = pool->buffer_size;
    pool->buffer      = static_cast<u8*>(virtual_alloc(pool->buffer_size));
  }

  temporary_pool* current_pool = pool;
  temporary_pool* last_pool = pool;
  while(current_pool) {
    if(current_pool->empty_size >= size) {
      s32 count = (s32) (current_pool->buffer_size - current_pool->empty_size);
      u32 index = (count > 0) ? (count / sizeof(u8)) : 0;

      buffer = &current_pool->buffer[index];
      current_pool->empty_size -= size;
      memset(buffer, 0, size);
      break;
    }
    
    last_pool = current_pool;
    current_pool = current_pool->next;
  }
  
  if(buffer == NULL) {
    temporary_pool* new_pool = static_cast<temporary_pool*>(malloc(sizeof(temporary_pool))); // @temporary: replace for dormammu alloc
    *new_pool = {};
    last_pool->next = new_pool;

    new_pool->buffer_size = ((size / 4096) + 1) * 4096;
    new_pool->empty_size  = new_pool->buffer_size - size;
    new_pool->buffer 	  = static_cast<u8*>(virtual_alloc(new_pool->buffer_size));

    buffer = new_pool->buffer;
  }
  
  return(buffer);
}

internal void
temporary_push(temporary_pool*& pool, void* data, size_t data_size) {
  void* memory_pool = temporary_alloc(pool, data_size);
  if(memory_pool) {
    memcpy(memory_pool, data, data_size);
  }
}

internal void
clear_temporary_pool(temporary_pool*& pool) {
  if(pool) {
    clear_temporary_pool(pool->next);
    virtual_free(pool->buffer, 0);
    free(pool); // @temporary: replace for dormammu alloc
  }
  pool = NULL;
}

// #include "string.h"

// @temporary
#define MEM_READ_BUFFER(_value, _buffer, _index, _size, _type, _size_count) \
  memcpy(&_value, &_buffer[_index], sizeof(_size) * _size_count);	  \
  _index += (sizeof(_size) * _size_count) / sizeof(_type);

#endif // !BASIC_H
