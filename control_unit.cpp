#include "control_unit.h"

//constructer
ControlUnit::ControlUnit(): mem_read(0), mem_write(0), reg_write(0), mem_to_reg(0), alu_src(0), reg_dst(0),
                            pc_src(0), alu_op(0), branch(0), data_hazard_mem_to_mem(0), nop(0), resolved(0), redo_decode(0), forwardA(0), forwardB(0), forwardC(0), exit(0){}   

void ControlUnit::setControlSignals(const uint32_t opcode, const uint32_t function_opcode = 0){
    if (opcode == R_TYPE && (function_opcode == ADD || function_opcode == SUB || function_opcode == AND ||
        function_opcode == OR || function_opcode == XOR || opcode == NOR || opcode == MFHI || opcode == MFLO)){
            reg_dst = 1;
            reg_write = 1;
            alu_op = 10;

    } else if (opcode == MULT || opcode == DIV) {
            reg_dst = 1;
            alu_op = 10;

    } else if (opcode == MFHI || opcode == MFLO) {
            reg_dst = 1;
            reg_write = 1;
            alu_op = 10;

    } else if (opcode == SLL || opcode == SRL) {
            reg_dst = 1;
            reg_write = 1;
            alu_src = 1;
            alu_op = 10;

    } else if (opcode == ADDI || opcode == ANDI || opcode == ORI || opcode == XORI || opcode == LUI || opcode == LI) {
            alu_src = 1;
            reg_write = 1;
            if (opcode == ADDI)
                alu_op = 0;
            else 
                alu_op = 11;

    } else if (opcode == BEQ || opcode == BNE || opcode == BGEZ || opcode == BGTZ || opcode == BLEZ || opcode == BLTZ) {
            branch = 1;
            alu_op = 1;

    } else if (opcode == J || opcode == JAL || opcode == JR) {
            branch = 1; // might change to jump

    } else if (opcode == SB || opcode == SH || opcode == SW) {
            mem_write = 1;
            alu_src = 1;

    } else if (opcode == LW || opcode == LB || opcode == LH) {
            alu_src = 1;
            mem_to_reg = 1;
            reg_write = 1;
            mem_read = 1;
    }
}


// set control signals for pipeline stages
// update this to take opcode, go by instruction type and then do a switch statement and go through every instruction to set
// the control signals accordingly with each specific instruction 
// the control signals accordingly with each specific instruction 
void ControlUnit::forward_control_signals(EX_MEM& ex_mem) {
    // setting control signals
    ex_mem.reg_write = reg_write;
    ex_mem.mem_write = mem_write;
    ex_mem.mem_to_reg = mem_to_reg;
    ex_mem.alu_src = alu_src;
    ex_mem.mem_read = mem_read;
    ex_mem.reg_dst = reg_dst;
    ex_mem.alu_op = alu_op;
    reset();
}

void ControlUnit::forward_control_signals_mem_wb(EX_MEM& ex_mem, MEM_WB& mem_wb) {
    // setting control signals
    mem_wb.reg_write = ex_mem.reg_write;     // RegWrite
    mem_wb.mem_to_reg = ex_mem.mem_to_reg;   
    mem_wb.mem_read = ex_mem.mem_read;
    mem_wb.mem_write = ex_mem.mem_write;
    mem_wb.alu_op = ex_mem.alu_op;
    reset_ex_mem(ex_mem);
}


void ControlUnit::reset(){
    mem_read = 0;      
    mem_write = 0;     
    reg_write = 0;     
    mem_to_reg = 0;    
    alu_src = 0;       
    reg_dst = 0;
//     pc_src = 0;
    alu_op = 0;
    branch = 0;
    data_hazard_mem_to_mem = 0;
    nop = 0;
}

void ControlUnit::reset_ex_mem(EX_MEM& ex_mem){
        ex_mem.reg_write = 0;
        ex_mem.mem_to_reg = 0;
        ex_mem.mem_read = 0;
        ex_mem.mem_write = 0;
        ex_mem.alu_op = 0;
}
