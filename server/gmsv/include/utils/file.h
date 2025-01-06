#ifndef __UTILS_FILE_H__
#define __UTILS_FILE_H__

#include <stdio.h>

typedef void (*CallbackFunction)(int *line_num, const char *line);

FILE* open_realop_file(const char* filename, int *use_relop_file);

int get_file_line_num(const char* filename);

int get_file_lines(const char* filename, int * line_num, CallbackFunction callback);

#endif

