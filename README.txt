-------------------------------------------------------------------
PNN ASSEMBLER - Version 1.0 - 16-bit Architecture
Integer standard: 2's complement form (16-bit)
Floating numbers standard: IEEE754 Half-Precision Standard (16-bit) 
-------------------------------------------------------------------

Instruction type:

[label :] OPCODE REG1 REG2 imm_data	;comment

Eg.
ADD R1 R2 R3 ;Add comment after a semicolon
label: MVI R1 560h

* Commas are allowed between register names and/or opcode.

Eg.
SUB R1, R2, R3

* Label names should be alphanumeric and must not contain any special characters other than '_'

* Object file structure:

xxxxxxxxxxxxxxxxx
16-bit binary code
xxxxxxxxxxxxxxxxx

--End of Machine Instructions and Constants
--Idetification Information:
-Filename = string
-Author = Me
-Creation Timestamp = Sun Apr 12 21:20:22 2015
-Total Instructions = INT
-Total Size = INT Bytes
--Relocation Dictionary:
-HEX ADD
-HEX ADD
-HEX ADD
--Start Address = HEX ADD
--End of Object file.

