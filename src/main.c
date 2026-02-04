#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "common.h" 
#include "utils/io_utils.h"
#include "decoder/decoder.h"
#include "decoder/opcodes.h"

u8 is_op_prefix(u8 opcode)
{
    return (opcode == 0x26 ||\
            opcode == 0x2E ||\
            opcode == 0x36 ||\
            opcode == 0x3E);
}

i32 main(i32 argc, u8 **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", PROGRAM_PATH);
        return (EXIT_FAILURE);
    }

    const char *filename = argv[1];

    int fd_in = open(filename, O_RDONLY);
    if (fd_in == -1)
    {
        perror(filename);
        return (EXIT_FAILURE);
    }

    u8 buffer[1024];
    ssize_t read_bytes = read(fd_in, buffer, 1024);
    close(fd_in);
    if (read_bytes == -1)
    {
        return (EXIT_FAILURE);
    }
    buffer[read_bytes] = '\0';

    printf("Read %lu bytes from: `%s`\n", read_bytes, filename);

    char const *filename_out = "out.asm";
    int fd_out = open(filename_out, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd_out == -1)
    {
        printf("Error: Cannot create a file\n");
        return (EXIT_FAILURE);
    }

    write_file_header(fd_out);
    if (read_bytes == 0)
    {
        printf("Nothing to decode\n");
    }

    t_arena *a = arena_create(LIFE_ARENA_SIZE);
    if (!a)
    {
        return 1;
    }

    u64 offset = 0;
    u8 opcode;
    t_ctx ctx = 
    {
        .a = a,
        .b = buffer,
        .fd = filename_out,
        .current_ip = offset,
        .seg_prefix = 0
    };

    u64 i = 0;
    while (i < read_bytes)
    {
        ctx.b = &buffer[i];
        opcode = ctx.b[0];
        u8 is_prefix = is_op_prefix(opcode);
        func_ptr handler = META_TABLE[opcode];
        offset = handler(&ctx);

        if (offset)
        {
            i += offset;
            ctx.current_ip += offset;
            arena_reset(ctx.a);
            if (!is_prefix)
            {
                ctx.seg_prefix = 0xFF;
            }
        }
        else
        {
            printf("Error: Infinite loop\n");
            break;
        }
    }

    close(fd_out);
    printf("Output written to: `%s`\n",filename_out);

    return (EXIT_SUCCESS);
}