#include "io_utils.h"

void write_file_header(i32 fd)
{
    write(fd, FILE_HEADER, strlen(FILE_HEADER));
}

void write_fmt_line_no_operands(t_ctx *ctx, u8 *mnemonic)
{
    u8 *line = strjoin_fmt(ctx->a, "%-7s", mnemonic);
    write(ctx->fd, line, strlen(line));
}

void write_fmt_line(t_ctx *ctx, u8 *mnemonic, u8 *operands)
{
    u8 *line = strjoin_fmt(ctx->a, "%-7s %s", mnemonic, operands);
    write(ctx->fd, line, strlen(line));
}

