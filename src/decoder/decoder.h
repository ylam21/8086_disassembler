#ifndef DECODER_H
#define DECODER_H

#include <stdio.h>
#include "../common.h"
#include "../utils/io_utils.h"
#include "../utils/string_utils.h"

size_t decode_bin_to_asm_16(i32 fd, u8 *buffer);

#endif