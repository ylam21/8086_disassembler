#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "common.h" 
#include "utils/io_utils.h"
#include "decoder/decoder.h"

int main(int argc, char **argv)
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

    ssize_t idx = 0;
    size_t offset = 0;
    while (idx < read_bytes)
    {
        offset = decode_bin_to_asm_16(fd_out, buffer + idx);
        idx += offset;
    }
    close(fd_out);

    printf("Output written to: `%s`\n",filename_out);

    return (EXIT_SUCCESS);
}