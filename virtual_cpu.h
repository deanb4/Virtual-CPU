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
#include "pipeline_registers.h"
#include "control_unit.h"

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

// ***** UPDATES *****
// create function that handles exception: takes flags check if exeption flag is true
// if yes then gets cause and epc register and throws exception printing the address of instruction 
// load program from given file path
// *******************

class CPU{

    private:
        ui32 sp;  // Stack pointer starts at this address 
        ui32 gp;  // Starting address for global variables  
        size_t cycles;
        size_t stalled_cycles;
        const ui32 MEMORY_SIZE = (1 << 31); // (2GB)
        // ui32 registers[NUM_REGISTERS]; // Registers
        // ui32 special_registers[NUM_SPECIAL_REGISTERS]; // special registers
        ui32 fp_registers[NUM_FP_REGISTERS]; // floating point registers
        bool halted; 
        bool dont_set;
        // ui32 pc; // program counter
        Flags flags; // flags for status
        std::unordered_map<i32, std::string> file_descriptors;
        ControlUnit control_unit;
        
        // pipeline registers
        IF_ID if_id;
        ID_EX id_ex;
        EX_MEM ex_mem;
        MEM_WB mem_wb;
        Wb_display wb_display; // for displaying pipeline stage
        
        void executeSyscall();
        void executeSyscall_pipeline();
        
    public:
        ui32 special_registers[NUM_SPECIAL_REGISTERS]; // special registers (remove from public)
        ui32 pc; // program counter (remove from public)
        ui32* memory; // Memory (remove from public)
        ui32 registers[NUM_REGISTERS]; // Registers (remove from public)
        CPU(); // constructor
        void loadProgram(const std::vector<ui32>&);
        void fetch();
        bool decodeExecute(ui32); // make no discard after it returns exceptions
        // void syscall(const ui32&); not used
        void run();
        void debug();
        void display_general_registers() const;
        void display_memory(ui32,ui32) const;

        // Pipeline functionality
        void run_pipeline();
        void fetch_pipeline();
        void execute(); // for pipeline
        void decode(); // for pipeline
        void mem();
        void WB();

        void debug_pipeline();
        void display_pipeline_registers();
        void display_pipeline();

        ~CPU(); // destructor

};

#include "virtual_cpu.cpp"
#endif
