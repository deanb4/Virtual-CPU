#include <iostream>
#include <vector>
#include "virtual_cpu.h"
#include "Assembler.h"

// Debug / Display info
#define DEBUG_DEMO 0
#define DISPLAY_REGISTERS 0
#define DISPLAY_MEMORY 0
#define DISPLAY_STACK 0

// program selection
#define CONTROL_HAZARD 0
#define HELLO_WORLD 1

// prototype programs
void helloWorld();
void dataHazard();
void controlHazard();

// **********Programs**********

// RAW Hazard Demo
std::string data_hazard = 
"addi $t1, $t1, 17\n"
"addi $t2, $t2, 2\n" 
"addi $t5, $t5, 17\n" 
"sub $t0, $t1, $t2\n"
"addi $v0, $v0, 10\n"
"syscall\n";

std::string hello_world = 
"addi $v0, $v0, 4\n"
"lui $a0, 5\n"
"syscall\n"
"addi $v0, $v0, 10\n"
"syscall\n";

// Control Hazard Demo
std::string branch_equal_not_equal = 
"addi $t0, $t0, 4\n"
"addi $t1, $t1, 4\n"
"beq $t0, $t1, 6\n"

"addi $v0, $v0, 4\n"
"lui $a0, 20\n"
"ori $a0, $a0, 20\n"
"syscall\n"

"addi $v0, $v0, 10\n"
"syscall\n"

"addi $v0, $v0, 4\n"
"lui $a0, 26\n"
"ori $a0, $a0, 26\n"
"syscall\n"

"addi $v0, $v0, 10\n"
"syscall\n";

// **************************

CPU cpu;

// ADD: add open/read file option

int main(){
    std::vector<uint32_t> program_binary;
    Assembler assembler(hello_world);
    int fail = assembler.encodeInstructions(program_binary);
    
    if (CONTROL_HAZARD)
        controlHazard();
    else if (HELLO_WORLD)
        helloWorld();

    cpu.loadProgram(program_binary);
    
    // Debug
    if (DEBUG_DEMO)
        cpu.debug_pipeline();
    else
        cpu.run_pipeline();
    
    // Display general info
    if (DISPLAY_REGISTERS)
        cpu.display_general_registers();
    
    if (DISPLAY_MEMORY)
        cpu.display_memory(0,15);

    if (DISPLAY_STACK)
        cpu.display_memory(STACK_MEMORY_BOUND-10,STACK_MEMORY_BOUND);


    return 0;
}


void helloWorld(){
    char message[] = "Hello World\0";
    memcpy(cpu.memory+5, message, sizeof(message)); 
}

void controlHazard(){
    char message[] = "Registers are not equal\0";
    memcpy(cpu.memory + 20, message, sizeof(message));
    char message1[] = "Registers are equal\0";
    memcpy(cpu.memory + 26, message1, sizeof(message1));
}
