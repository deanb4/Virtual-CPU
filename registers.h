#ifndef REGISTERS_H
#define REGISTERS_H

// 32 registers 
enum Registers {
    ZERO,  // $zero register
    AT,  // reserved for assembler
    V0, V1, // function return values
    A0, A1, A2, A3, // arguments
    T0, T1, T2, T3, T4, T5, T6, T7, // Temporary 
    S0, S1, S2, S3, S4, S5, S6, S7, // saved registers
    T8, T9, // temporary
    K0, K1, // kernal
    GP, // global pointer
    SP, // stack pointer
    FP,  // frame pointer
    RA, // return address
    NUM_REGISTERS   // total number of registers
};

// special registers
enum SpecialRegisters {
    HI, LO, STATUS, 
    EPC, CAUSE, 
    NUM_SPECIAL_REGISTERS
};

// Floating point registers
enum FloatingPointRegisters {
    F0, F1, F2, F3, F4, F5, F6, F7,
    F8, F9, F10, F11, F12, F13, F14, F15,
    F16, F17, F18, F19, F20, F21, F22, F23,
    F24, F25, F26, F27, F28, F29, F30, F31,
    NUM_FP_REGISTERS
};

#endif