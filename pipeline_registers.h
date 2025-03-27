#ifndef PIPELINE_REGISTERS
#define PIPELINE_REGISTERS

#include <cstdint>

// IF/ID Register (Between IF & ID)
struct IF_ID {
    uint32_t instruction;
    uint32_t next_instruction;

    IF_ID(): instruction(0), next_instruction(0){}

};

// ID/EX Register (Between ID & EX)
struct ID_EX {
    uint32_t opcode;
    uint32_t instruction;
    uint32_t rs;
    uint32_t rt;
    uint32_t rd;
    uint32_t immediate;
    uint32_t shamt;
    uint32_t function_opcode;
    uint32_t jump_address;
    uint32_t syscall_code;
    uint32_t link_address;

    // Add control signals
    uint8_t mem_read;      // MemRead
    uint8_t mem_write;     // MemWrite
    uint8_t reg_write;     // RegWrite
    uint8_t mem_to_reg;    // MemToReg (for load instruction)
    uint8_t alu_src;       // ALUSrc
    uint8_t reg_dst;       // ALUSrc

    ID_EX(): opcode(0), instruction(0), rs(0), rt(0), rd(0), immediate(0), shamt(0),
            function_opcode(0), jump_address(0), syscall_code(0), link_address(0),
            mem_read(0), mem_write(0), reg_write(0), mem_to_reg(0), alu_src(0), reg_dst(0){}
    
};

// EX/MEM Register (Between EX & MEM)
struct EX_MEM {
    uint32_t opcode;
    uint32_t alu_result;
    uint32_t store_val;
    uint32_t store_val2;
    uint32_t destination_register;
    uint32_t jump_address;
    uint32_t link_address;
    uint32_t syscall_code;
    uint32_t arg1, arg2, arg3, arg4;

    // Add control signals
    uint8_t mem_read;      // MemRead
    uint8_t mem_write;     // MemWrite
    uint8_t reg_write;     // RegWrite
    uint8_t mem_to_reg;    // MemToReg
    uint8_t alu_src;       // ALUSrc
    uint8_t reg_dst;       // ALUSrc

    EX_MEM(): opcode(0), alu_result(0), store_val(0), store_val2(0), destination_register(0), 
              jump_address(0), link_address(0), syscall_code(0), arg1(0), arg2(0), arg3(0),
              arg4(0), mem_read(0), mem_write(0), reg_write(0), mem_to_reg(0), alu_src(0), reg_dst(0){}
};

// MEM/WB Register (Between MEM & WB)
struct MEM_WB {
    uint32_t opcode;
    uint32_t alu_result;
    uint32_t destination_register; 
    uint32_t mem_data; 

    // Add control signals
    uint8_t reg_write;     // RegWrite
    uint8_t mem_to_reg;    // MemToReg (to choose between ALU result or memory data)

    MEM_WB(): opcode(0), alu_result(0), destination_register(0), mem_data(0), reg_write(0),  mem_to_reg(0){}
};


// just for displaying all 5 levels of pipeline
struct Wb_display {
    uint32_t opcode;
    Wb_display(): opcode(0){}
};

#endif

