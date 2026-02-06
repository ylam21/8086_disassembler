# 8086 Disassembler

A standard 8086 instruction disassembler that converts binary machine code into readable assembly (`.asm`) source.<br>
All supported operation codes (OPCODES) are defined in `src/decoder/opcodes.c`. The decoding logic handles the variable-length instruction encoding of the 8086 processor, mapping the 256 opcode combinations to their respective mnemonics and operands.
## Context
This project is a solution for one of the homework assignments in Casey Muratori's [Performance-Aware Programming](https://www.computerenhance.com/) course.<br>

## References
[Intel 8086 Family User's Manual October 1979](https://archive.org/details/bitsavers_intel80869lyUsersManualOct79_62967963/page/n1/mode/2up) [archive.org]<br>

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
The disassembled output will be written to `out.asm` file in the current directory.
### Example:
```sh
bin/decode8086 examples/listing_0037_single_register_mov
```