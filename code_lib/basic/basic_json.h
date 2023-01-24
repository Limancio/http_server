#ifndef BASIC_JSON_H
#define BASIC_JSON_H

#include "basic/basic_lexer.h"

const s32 OPEN_OBJECT_TOKEN  = '{';
const s32 CLOSE_OBJECT_TOKEN = '}';
const s32 OPEN_ARRAY_TOKEN   = '[';
const s32 CLOSE_ARRAY_TOKEN  = ']';
const s32 DIV_TOKEN	         = ',';
const s32 QUOTES_TOKEN 	     = '"';
const s32 EQUALS_TOKEN 	     = ':';

enum json_element_type {
  JSON_TYPE_STRING = 0,
  JSON_TYPE_NUMBER,
  JSON_TYPE_OBJECT,
  JSON_TYPE_ARRAY,
  JSON_TYPE_BOOL,
  JSON_TYPE_NULL
};

enum json_parse_type {
  PARSE_VALUE = 0,
  PARSE_KEY
};

struct element_list;
  
struct element {
  element* parent;

  string key;
  string value;
  
  element_list* child_list;
  u32 child_count;
};
create_list(element, child_list, element, 32)

internal element*
get_json_element_by_key(element* at, string key) {
  element* result = NULL;
  if(string_equals(at->key, key)) {
    result = at;
  } else {
    element_list* child_list = at->child_list;
    while(child_list) {
      element* next = &child_list->element;
      result = get_json_element_by_key(next, key);
      if(result) break;
      
      child_list = child_list->next;
    }
  }

  return(result);
}

internal string
get_json_value(element* at, string key) {
  string result = {};
  
  if(string_equals(at->key, key)) {
    result = at->value;
  } else {
    element_list* child_list = at->child_list;
    while(child_list) {
      element* next = &child_list->element;
      
      result = get_json_value(next, key);
      if(string_exist(result)) break;
      
      child_list = child_list->next;
    }
  }

  return(result);
}
  
internal void
read_json_data(element_list_arena* element_list_arena, element* root_element, string json_data) {
  lexer lexer = {};
  lexer_init(&lexer, json_data.data, &json_data.data[json_data.count - 1], true);
  
  element* active_element = root_element;
  json_parse_type parse_type = PARSE_KEY;
  json_element_type element_type = JSON_TYPE_OBJECT;
  
  while(lexer_get_token(&lexer)) {
    if(lexer.token == LEX_PARSE_ERROR) {
      log_error("PARSE ERROR\n");
      break;
    }
    
    switch(lexer.token) {
    case OPEN_ARRAY_TOKEN: 
    case OPEN_OBJECT_TOKEN: {
      u32 index = active_element->child_count;
      active_element->child_count += 1;
      
      element* new_element = get_child_at(element_list_arena, active_element, index);
      new_element->parent = active_element;
      active_element = new_element;
      
      if(lexer.token == OPEN_ARRAY_TOKEN) {
	element_type = JSON_TYPE_ARRAY;
	parse_type = PARSE_VALUE;
      } else {
	element_type = JSON_TYPE_OBJECT;
	parse_type = PARSE_KEY;
      }
    } break;
    case CLOSE_ARRAY_TOKEN: 
    case CLOSE_OBJECT_TOKEN: {
      // @warning: this is not good.
      parse_type = PARSE_KEY;
      element_type = JSON_TYPE_OBJECT;
      active_element = active_element->parent;
    } break;
    case DIV_TOKEN: {
      element* parent = (active_element->parent) ? active_element->parent : active_element;
      
      u32 index = parent->child_count;
      parent->child_count += 1;
      
      element* new_element = get_child_at(element_list_arena, parent, index);
      new_element->parent = parent;
      active_element = new_element;
      
      if(element_type == JSON_TYPE_ARRAY) {
	parse_type = PARSE_VALUE;
      } else {
	parse_type = PARSE_KEY;
      }
    } break;
    case EQUALS_TOKEN: {
      parse_type = PARSE_VALUE;
    } break;
    case LEX_STRING:
    case LEX_ID: {
      switch(parse_type) {
      case PARSE_KEY: {
	active_element->key = lexer.string_token;
      } break;
      case PARSE_VALUE: {
	active_element->value = lexer.string_token;
      } break;
      }
    } break;
    }
  }
}

#endif // !BASIC_JSON_H
