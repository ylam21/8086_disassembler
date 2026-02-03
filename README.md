# 8086 Disassembler

A lightweight 8086 instruction disassembler that converts binary machine code into readable assembly (`.asm`) source.<br>
**NOTE:** the program can only decode the `mov` instruction.<br>

## References
[Intel 8086 Family User's Manual October 1979](https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf) [edx.org]<br>
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