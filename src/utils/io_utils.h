#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include "../common.h"


void write_file_header(i32 fd);
void write_line(i32 fd, char *instr, char *dest, char *src);

#endif