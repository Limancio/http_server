#ifndef LOGGER_H
#define LOGGER_H

// @incomplete:
#include <stdio.h>

#define log(...) printf(__VA_ARGS__);
#define log_string(_string) fwrite(_string.data, sizeof(s8), _string.count, stdout); \
  			    fprintf(stdout, "");

#define log_info(...) printf("[INFO]: "); printf(__VA_ARGS__);
#define log_error(...) printf("[ERROR]: ");  printf(__VA_ARGS__);

#endif // !LOGGER_H
