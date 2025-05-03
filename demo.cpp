#include <iostream>
#include <vector>
#include "virtual_cpu.h"
#include "Assembler.h"

// Debug - Display info
#define DEBUG_DEMO 0
#define DISPLAY_REGISTERS 1
#define DISPLAY_MEMORY 1
#define DISPLAY_STACK 1

// program selection
#define CONTROL_HAZARD 1
#define HELLO_WORLD 0

// prototype programs
void helloWorld();
void dataHazard();
void controlHazard();

// **********Programs**********

// data hazard
std::string d_hazard = 
"li $t3, 8\n"
"li $t1, 10\n"
"sw $t1, 0($t3)\n"
"li $v0, 10\n"
"syscall\n";


// RAW Hazard Demo (expect 42 in $t5)
std::string data_hazard = 
"addi $t0, $t0, 5\n"
"addi $t1, $t1, 17\n" 
"sub $t2, $t1, $t0\n"
"addi $t3, $t3, 15\n"
"add $t4, $t3, $t2\n"
"add $t5, $t4, $t3\n"
"addi $v0, $v0, 10\n"
"syscall\n";

// expect 3 in $t1
std::string WWR_data_hazard = 
"addi $t0, $t0, 1\n"
"addi $t0, $t0, 2\n"
"add $t1, $t0, $t5\n"
"li $v0, 10\n"
"syscall\n";

// expect 1 in $t1
std::string skipped_cycle_data_hazard = 
"addi $t0, $t0, 1\n"
"addi $t0, $t0, 0\n"
"addi $t0, $t0, 0\n"
"add $t1, $t0, $t5\n"
"li $v0, 10\n"
"syscall\n";

// expect 3 in $t2
std::string data_hazard_two_inst_writing_diff_reg = 
"addi $t0, $t0, 1\n"
"addi $t1, $t1, 2\n"
"add $t2, $t1, $t0\n"
"li $v0, 10\n"
"syscall\n";

// expect 10 in $t1
std::string double_reg_reuse_data_hazard = 
"addi $t0, $t0, 5\n"
"add $t1, $t0, $t0\n"
"li $v0, 10\n"
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

// FIXME: NOT WORKING ******
std::string array_program = 
"addi $t0, $t0, 0\n" // counter
"addi $t1, $t1, 6\n" // loop bound
"addi $t4, $t4, 1\n" 
"beq $t0, $t1, 4\n"
"sub $sp, $sp, $t4\n"
"sw $t0, 0($sp)\n" // place in array
"addi $t0, $t0, 1\n" // add 1 to counter
"j 3\n"
"addi $v0, $v0, 10\n"
"syscall\n";

std::string test1 = 
"addi $t0, $t0, 5\n" // fix fowarding issue here
"addi $t1, $t1, 7\n"
"add $t2, $t1, $t0\n" // only works when t1 is first
"addi $v0, $v0, 10\n"
"syscall\n";

std::string stack_test = 
"addi $t0, $t0, 5\n"
"addi $t1, $t1, 3\n"
"sub $sp, $sp, $t1\n"
"sw $t0, 0($sp)\n"
"addi $v0, $v0, 10\n"
"syscall\n";

// FIXME: *****debug add forward for store word !!*****
// add conditions 
// if wb rd == ex rd store val is wb rd
// if mem rd == ex rs; ex rs = mem_rd
std::string sw_test = 
"li $t0, 7\n"
"li $t1, 10\n"
"sw $t0, 0($t1)\n"
"addi $v0, $v0, 10\n"
"syscall\n";

std::string sw_data_hazard_no_load = 
"addi $t0, $t0, 3\n"
"addi $t1, $t1, 10\n"
"sw $t0, 0($t1)\n"
"addi $v0, $v0, 10\n"
"syscall\n";


// TEST 2: Store to computed address (data hazard on $t0)
std::string sw_data_hazard_test2 = 
"addi $t1, $t1, 20\n"
"addi $t0, $t0, 8\n"
"sw $t1, 4($t0)\n" 
"li $v0, 10\n"
"syscall\n";


// TEST 3: Store using same reg for data and address
// FIXME (left off here not working)
std::string sw_data_hazard_test3 = 
"addi $t0, $t0, 8\n" 
"sw $t0, 0($t6)\n"
"li $v0, 10\n"
"syscall\n";
// **************************

CPU cpu;

// ADD: add open/read file option

// // stack program li_$t0_5 ,addi_$sp_$sp_minus_4,sw_$t0_0_$sp, lw_$t0_0_$sp,addi_$sp_$sp_add_4
uint32_t li_$t0_5 = 0xE4080005;
uint32_t addi_$sp_$sp_minus_4 = 0x23BDFFFC;
uint32_t sw_$t0_0_$sp = 0xAFA80001;
uint32_t lw_$t0_0_$sp = 0x8FA20000;
uint32_t addi_$sp_$sp_add_4 = 0x23BD0004;

int main(){
    std::vector<uint32_t> program_binary;
    std::vector<uint32_t> program = {addi_$sp_$sp_minus_4,sw_$t0_0_$sp,0xE402000A, 0xFC000000};
    Assembler assembler(sw_data_hazard_test3);
    int fail = assembler.encodeInstructions(program_binary);
  
    if (CONTROL_HAZARD)
        controlHazard();
    else if (HELLO_WORLD)
        helloWorld();

    cpu.loadProgram(program_binary);
    cpu.registers[T6] = 8;
    // Debug
    if (DEBUG_DEMO)
        cpu.debug_pipeline();
    else
        cpu.run_pipeline();
    // cpu.run();

    // Display general info
    if (DISPLAY_REGISTERS)
        cpu.display_general_registers();
    
    if (DISPLAY_MEMORY)
        std::cout << "Memory" << std::endl;
        cpu.display_memory(0,15);

    if (DISPLAY_STACK)
        std::cout << "STACK" << std::endl;
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
