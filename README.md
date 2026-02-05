# 8086 Disassembler

A 8086 instruction disassembler that converts binary machine code into readable assembly (`.asm`) source.<br>
All covered operation (instruction) codes (OPCODES) are listed under `src/decoder/opcodes.c` source file. All OPCODES are encoded in the 1st byte of the instruction - there is 256 different combinations, however, many instrucitons share the same logic. You can see the `src/decoder/opcodes.c` file where the patterns in the encoding logic are easily seen.

## References
[Intel 8086 Family User's Manual October 1979](https://archive.org/details/bitsavers_intel80869lyUsersManualOct79_62967963/page/n1/mode/2up) [archive.org]<br>
This project is a solution for one of the homework assignments in Casey Muratori's [Performance-Aware Programming](https://www.computerenhance.com/) course.<br>

## Usage

### Compilation
Use `Makefile` to build the executable:
```sh
make all
```
### Run
Provide the path to an 8086 binary file as an argument:
```sh
bin/decode8086 <filename>
```
### Example:
```sh
bin/decode8086 examples/listing_0037_single_register_mov
```