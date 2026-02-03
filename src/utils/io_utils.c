#include "io_utils.h"

static void write_string_fd(i32 fd, char *str)
{
    write(fd, str, strlen(str));
}

void write_file_header(i32 fd)
{
    write_string_fd(fd, FILE_HEADER);
}


void write_line(i32 fd, char *instr, char *dest, char *src)
{
    write_string_fd(fd, instr);
    write_string_fd(fd, " ");
    write_string_fd(fd, dest);
    write_string_fd(fd, ", ");
    write_string_fd(fd, src);
    write_string_fd(fd, "\n");
}