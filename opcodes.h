#ifndef OPCODES_H
#define OPCODES_H

#include <cstdint>

enum Opcode : uint8_t {
    // R-Type Instructions (opcode = 0)
    R_TYPE = 0x00,
    ADD   = 0x20,   // Function code for add
    SUB   = 0x22,   // Function code for subtract
    MULT  = 0x18,   // Function code for multiply
    DIV   = 0x1A,   // Function code for divide
    MFHI  = 0x10,   // Function code for move from Hi
    MFLO  = 0x12,   // Function code for move from Lo
    AND   = 0x24,   // Function code for bitwise AND
    OR    = 0x25,   // Function code for bitwise OR
    XOR   = 0x26,   // Function code for bitwise XOR
    NOR   = 0x27,   // Function code for bitwise NOR
    SLL   = 0x00,   // Function code for shift left logical
    SRL   = 0x02,   // Function code for shift right logical
    JALR  = 0x09,   // Function code for jump and link register

    // I-Type Instructions
    ADDI  = 0x08,   // Opcode for add immediate
    ANDI  = 0x0C,   // Opcode for and immediate
    ORI   = 0x0D,   // Opcode for or immediate
    XORI  = 0x0E,   // Opcode for xor immediate
    LW    = 0x23,   // Opcode for load word
    SW    = 0x2B,   // Opcode for store word
    LB    = 0x50,   // Opcode for load byte
    LH    = 0x21,   // Opcode for load halfword
    SB    = 0x28,   // Opcode for store byte
    SH    = 0x29,   // Opcode for store halfword
    BEQ   = 0x04,   // Opcode for branch if equal
    BNE   = 0x05,   // Opcode for branch if not equal
    BGEZ  = 0x01,   // Opcode for branch if greater than or equal to zero (special handling for rt)
    BGTZ  = 0x07,   // Opcode for branch if greater than zero
    BLEZ  = 0x06,   // Opcode for branch if less than or equal to zero
    BLTZ  = 0xCF,   // Opcode for branch if less than zero (special handling for rt) (**** change value)
    LUI   = 0x0F,   // Opcode for load upper immediate

    // J-Type Instructions
    J     = 0x33,   // Opcode for jump
    JAL   = 0x03,   // Opcode for jump and link
    JR    = 0x0B,    // Function code for jump register (R-type with funct 0x08)

    //Syscall
    SYSCALL = 0xAB,
};

#endif