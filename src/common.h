#ifndef COMMON_H
#define COMMON_H

#define PROGRAM_PATH "bin/decode8086"
#define FILE_HEADER "bits 16\n\n"
#define LIFE_ARENA_SIZE 1024
#define EXIT_ERROR 0u
#define OFFSET_OPCODE_NOT_USED 1u
#define ACC_BYTE "al"
#define ACC_WORD "ax"

#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#endif