#ifndef CONTROL_UNIT
#define CONTROL_UNIT

#include <cstdint>

class ControlUnit{
    public:
        uint8_t mem_read;      // MemRead
        uint8_t mem_write;     // MemWrite
        uint8_t reg_write;     // RegWrite
        uint8_t mem_to_reg;    // MemToReg (for load instruction)
        uint8_t alu_src;       // ALUSrc
        uint8_t reg_dst;
        uint8_t pc_src;
        uint8_t alu_op;
        uint8_t branch;
        uint8_t data_hazard_mem_to_mem;
        uint8_t forwardA;
        uint8_t forwardB;
        uint8_t nop;
        uint8_t resolved;
        uint8_t redo_decode;
        uint8_t exit;

        // constructer
        ControlUnit();
        void setControlSignals(const uint32_t, const uint32_t);
        void forward_control_signals(EX_MEM&);
        void forward_control_signals_mem_wb(EX_MEM&, MEM_WB&);
        void reset();

};

#include "control_unit.cpp"
#endif
