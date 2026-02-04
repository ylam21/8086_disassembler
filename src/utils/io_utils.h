#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include "../common.h"
#include "string_utils.h"
#include "decoder/opcodes.h"

void write_file_header(i32 fd);
void write_fmt_line(t_ctx *ctx, u8 *mnemonic, u8 *operands);
void write_fmt_line_no_operands(t_ctx *ctx, u8 *mnemonic);

#endif