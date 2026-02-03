# 8086 Disassembler

A lightweight 8086 instruction disassembler that converts binary machine code into readable assembly (`.asm`) source.
**NOTE:** the program can only decode the `mov` instruction.

## References
[Intel 8086 Family User's Manual October 1979](https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf)
This project is a solution for the homework assignments in Casey Muratori's [Performance-Aware Programming](https://www.computerenhance.com/) course.

## Usage

### Compilation
Use `gcc` to build the executable:
```sh
gcc disassembler.c arena.c itoa.c -o decode8086
```
### Run
Provide the path to an 8086 binary file as an argument:
```sh
./decode8086 <filename>
```
### Example:
```sh
./decode8086 examples/listing_0037_single_register_mov
```