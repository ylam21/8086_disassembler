#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include "../common.h"


void write_file_header(i32 fd);
void write_string_fd(i32 fd, u8 *str);
void write_line(i32 fd, u8 *mnemonic, u8 *dest, u8 *src);
void write_line_str(i32 fd, u8 *mnemonic, u8 *str);

#endif