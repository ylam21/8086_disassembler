#include "io_utils.h"

void write_string_fd(i32 fd, u8 *str)
{
    write(fd, str, strlen(str));
}

void write_file_header(i32 fd)
{
    write_string_fd(fd, FILE_HEADER);
}

void write_line_str(i32 fd, u8 *mnemonic, u8 *str)
{
    write_string_fd(fd, mnemonic);
    write_string_fd(fd, " ");
    write_string_fd(fd, str);
    write_string_fd(fd, "\n");
}

void write_line(i32 fd, u8 *mnemonic, u8 *dest, u8 *src)
{
    write_string_fd(fd, mnemonic);
    write_string_fd(fd, " ");
    write_string_fd(fd, dest);
    write_string_fd(fd, ", ");
    write_string_fd(fd, src);
    write_string_fd(fd, "\n");
}