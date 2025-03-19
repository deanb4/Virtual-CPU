#ifndef VIRTUAL_CPU
#define VIRTUAL_CPU

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <fstream>
#include <random>
#include <unordered_map>
#include "flags.h"
#include "opcodes.h"
#include "utils.h"
#include "registers.h"
#include "syscalls.h"

#define TEXT_MEMORY 0x00000000
#define TEXT_MEMORY_BOUND 0x000FFFFF
#define GLOBAL_DATA_MEMORY 0x00010000
#define HEAP_MEMORY  0x00400000   
#define HEAP_MEMORY_BOUND 0x20000000
#define STACK_MEMORY 0x3F000000   
#define STACK_MEMORY_BOUND 0x3FFFFFFF
#define KERNEL_MEMORY 0x7F000000   
#define ui32 uint32_t
#define i32 int32_t
#define ui8 uint8_t
#define ui16 uint16_t

class CPU{

    private:
        ui32 sp;  // Stack pointer starts at this address 
        ui32 gp;  // Starting address for global variables  
        const ui32 MEMORY_SIZE = (1 << 31); // (2GB)
        // ui32 registers[NUM_REGISTERS]; // Registers
        ui32 special_registers[NUM_SPECIAL_REGISTERS]; // special registers
        ui32 fp_registers[NUM_FP_REGISTERS]; // floating point registers
        bool halted; 
        ui32 pc; // program counter
        Flags flags; // flags for status
        std::unordered_map<i32, std::string> file_descriptors;
        
        
    public:
        ui32* memory; // Memory (remove from public)
        ui32 registers[NUM_REGISTERS]; // Registers (remove from public)
        CPU(); // constructor
        void loadProgram(const std::vector<ui32>&);
        void fetch();
        bool decodeExecute(ui32); // make no discard after it returns exceptions
        void syscall(const ui32&);
        void executeSyscall();
        void run();
        ~CPU(); // destructor

};

#include "virtual_cpu.cpp"
#endif
