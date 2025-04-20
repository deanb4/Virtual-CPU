#include "virtual_cpu.h"

// Constructor
CPU::CPU(): memory(new ui32[MEMORY_SIZE]()), registers{0}, special_registers{0},
            fp_registers{0}, sp(STACK_MEMORY_BOUND), gp(GLOBAL_DATA_MEMORY),pc(TEXT_MEMORY), halted(false), cycles(0), stalled_cycles(0), dont_set(false){
                // Initializes Memory to 1GB, registers to 0, the sp and gp to respective memory locations,
                // pc to 0 and halted to false
            registers[Registers::SP] = sp;
            registers[Registers::GP] = gp;
            registers[SpecialRegisters::PC] = pc;
}


/****************************************
* ## Text Segment (code):
* 0x00000000 - 0x000FFFFF (1MB)
* 0 - 1,048,575
* Load program into text section of memory
****************************************/
void CPU::loadProgram(const std::vector<ui32>& program){
    ui32 i = TEXT_MEMORY;
    for (const auto &instruction : program){
        if (i <= TEXT_MEMORY_BOUND) // memory bounds check 
            memory[i++] = instruction;
        else    
            // pass error or throw exception
            throw std::out_of_range("Overflow of text segment: Program exceeds memory bounds.");
    }
    pc = TEXT_MEMORY;
    special_registers[SpecialRegisters::PC] = pc;
}

// Fetch Instruction from memory and pass to decodeExecute
void CPU::fetch(){
    ui32 instruction = memory[pc];
    std::cout << "PC: " << pc << std::endl;
    pc++;
    special_registers[SpecialRegisters::PC] = pc;
    decodeExecute(instruction);
}

// display registers
void CPU::display_general_registers() const{
     ui32 i = 0;
     for (auto j : registers){
         std::cout << utils::get_general_register(i) << ": " << j <<  "\n";
         i++;
     }
}

// display memory
void CPU::display_memory(ui32 start, ui32 end) const{
    for (int i =start; i < end; i++){
        std::cout << memory[i] << "\n";
    }
}

void CPU::debug(){
    char next = 'r';
    char user_input;
    while (!halted){
        std::cout << "Enter r for next instruction, q to quit: ";
        std::cin >> user_input;
        std::cout << "\n";
        if (next == user_input){
            fetch();
            display_general_registers();
        } else if (next == 'q'){
            halted = true;
        }
    }
    std::cout << "Exiting" << "\n";
}

// step by step debugging, displays registers and pipeline overview
void CPU::debug_pipeline(){
    char next = 'r';
    char user_input;
    size_t cycles = 0; 
    while (!halted){
        std::cout << "Enter r for next instruction, q to quit: ";
        std::cin >> user_input;
        std::cout << "\n";
        if (next == user_input){
            WB();
            mem();
            execute();
            decode();
            fetch_pipeline();
            cycles++;
            // display registers and pipeline
            display_general_registers();
            display_pipeline_registers();
            display_pipeline();
            std::cout << "cycles: " << cycles << "\n";
            std::cout << "stalled cycles: " << stalled_cycles << "\n";
        } else if (user_input == 'q'){
            halted = true;
        }
    }
    std::cout << "Exiting" << "\n";
}

// display the instructions currently in pipeline and at what stage
void CPU::display_pipeline(){
    std::cout << "| " << utils::get_opcode(((if_id.instruction) >> 26)) << " |" <<  "| " << utils::get_opcode(id_ex.opcode) << " |" <<
    "| " << utils::get_opcode(ex_mem.opcode) << " |" << "| " << utils::get_opcode(mem_wb.opcode) << " |" << "| " << utils::get_opcode(wb_display.opcode) << " |" << std::endl;
}

// display current state of the registers in between each pipeline stage
void CPU::display_pipeline_registers(){
    std::cout << "------------PIPELINE-----------------" << "\n\n";
    std::cout << "IF_ID REGISTER: " << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "instruction: " << if_id.instruction << std::endl;
    std::cout << "pc: " << if_id.next_instruction << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "ID_EX REGISTER: " << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "opcode: " << id_ex.opcode << std::endl;
    std::cout << "instruction: " << id_ex.instruction << std::endl;
    std::cout << "rs: " << id_ex.rs << std::endl;
    std::cout << "rt: " << id_ex.rt << std::endl;
    std::cout << "rd: " << id_ex.rd << std::endl;
    std::cout << "imm: " << id_ex.immediate << std::endl;
    std::cout << "shamt: " << id_ex.shamt << std::endl;
    std::cout << "function opcode: " << id_ex.function_opcode << std::endl;
    std::cout << "jump address: " << id_ex.jump_address << std::endl;
    std::cout << "syscall code: " << id_ex.syscall_code << std::endl;
    std::cout << "link address: " << id_ex.link_address << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "EX_MEM REGISTER: " << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "alu result: " << ex_mem.alu_result << std::endl;
    std::cout << "store val: " << ex_mem.store_val << std::endl;
    std::cout << "store val2: " << ex_mem.store_val2 << std::endl;
    std::cout << "destination register: " << ex_mem.destination_register << std::endl;
    std::cout << "jump address: " << ex_mem.jump_address << std::endl;
    std::cout << "link address: " << ex_mem.link_address << std::endl;
    std::cout << "syscall code: " << ex_mem.syscall_code << std::endl;
    std::cout << "arg1: " << ex_mem.arg1 << std::endl;
    std::cout << "arg2: " << ex_mem.arg2 << std::endl;
    std::cout << "arg3: " << ex_mem.arg3 << std::endl;
    std::cout << "arg4: " << ex_mem.arg4 << std::endl;
    std::cout << "arg4: " << ex_mem.rs_copy << std::endl;
    std::cout << "arg4: " << ex_mem.rt_copy << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "MEM_WB REGISTER: " << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "alu result: " << mem_wb.alu_result << std::endl;
    std::cout << "destination register: " << mem_wb.destination_register << std::endl;
    std::cout << "memory data: " << mem_wb.mem_data << std::endl;
    std::cout << "memory data copy: " << mem_wb.mem_data_copy << std::endl;
    std::cout << "memory data copy1: " << mem_wb.mem_data_copy1 << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    /*
    
        uint32_t alu_result;
    uint32_t destination_register; 
    uint32_t mem_data; 

    */
}

// run cpu
void CPU::run(){
    while (!halted){
        fetch();
    }
}

// run pipeline
void CPU::run_pipeline(){
    while (!halted){
        if (control_unit.nop == 1)
            stalled_cycles +=1;
        WB();
        mem();
        execute();
        decode();
        fetch_pipeline();
        cycles++;
    }
    std::cout << "cycles: " << cycles << "\n";
    std::cout << "stalled cycles: " << stalled_cycles << "\n";
}

// fetch pipeline ** check
void CPU::fetch_pipeline(){
    if (control_unit.nop == 0) {
        ui32 instruction = memory[pc];
        // check for branch. If pc src is 1 change pc to jump address and flush if_id and id_ex
        if (control_unit.pc_src == 1){ 
            pc = ex_mem.link_address;
            // std::cout << "PC: " << ex_mem.link_address << std::endl; // debug
            instruction = memory[pc];
            control_unit.pc_src = 0;
            if_id.flush();
            id_ex.flush(); // might need to remove**
        } else {
            pc++;
        }
        special_registers[SpecialRegisters::PC] = pc;
        if_id.instruction = instruction;
        if_id.next_instruction = pc;
        if_id.link_address = pc; // for branch 
    }
}


// decode function for pipeline
// set all control signals here!!!!!!!!!!!!
void CPU::decode(){
    if (control_unit.nop == 0) {
        ui32 instruction = 0;
        size_t current_cycle = 0;
        if (control_unit.redo_decode == 1){
            instruction = memory[pc-1];
            control_unit.redo_decode = 0;
        } else {
            instruction = if_id.instruction; // get instruction from pipeline register
        }
        ui32 opcode = instruction >> 26;
        id_ex.link_address = if_id.link_address; // for branch
        if (opcode == R_TYPE){
            ui32 rs = instruction >> 21 & 0x1F; // source register
            ui32 rt = instruction >> 16 & 0x1F;// target register
            ui32 rd = instruction >> 11 & 0x1F; // destination register
            ui32 shamt = instruction >> 6 & 0x3F;  // shift amount
            ui32 function_opcode = instruction & 0x3F; // extract function code from first 6 bits

            // adding to pipeline registers
            id_ex.opcode = opcode;
            id_ex.rs = registers[rs];
            id_ex.rd = rd;
            id_ex.rt = registers[rt];
            id_ex.shamt = shamt;
            id_ex.rs_reg = rs;
            id_ex.rt_reg = rt;
            id_ex.function_opcode = function_opcode;
            id_ex.rd_reg = 0;

            // data hazard load matching operand stall******** take actual value from reg ** fix
            ex_mem.rs_copy = registers[rs];
            ex_mem.rt_copy = registers[rt];
            ex_mem.rd_copy = rd;

            //  if (control_unit.resolved == 0 && mem_wb.mem_read == 1 && ((if_id.instruction >> 21 & 0x1F) == mem_wb.alu_result) || ((if_id.instruction >> 16 & 0x1F) == mem_wb.alu_result)){
            // detect stall condition FIXME triggering when not supposed to
            if (id_ex.rs_reg != 0 || id_ex.rd_reg != 0) {
                if (control_unit.resolved == 0 && ex_mem.mem_read == 1 && id_ex.opcode != SYSCALL &&  // not when syscall because it would trigger when I didint want to like syscall before addi
                   (id_ex.rs_reg == ex_mem.destination_register || id_ex.rd_reg == ex_mem.destination_register)){
                    std::cout << "stall condition triggered" << std::endl;
                    control_unit.resolved = 1; // not resolved
                    mem_wb = {}; // might change to id_ex try it
                    control_unit.nop = 1;
                    if_id = {};
                    control_unit.reg_write = 1;
                }
            }
            
            // setting control signals (update to go by instruction)
            control_unit.setControlSignals(opcode,function_opcode);
            // move signals to pipeline register 
            control_unit.forward_control_signals(ex_mem);
            
    

        } else if (opcode == J || opcode == JAL){
            id_ex.opcode = opcode;
            id_ex.instruction = if_id.instruction; // might need to change
            // setting control signals
            control_unit.setControlSignals(opcode);
            control_unit.forward_control_signals(ex_mem);
            if (opcode == JAL){
                // Store return address for JAL (to be written to $ra)
                id_ex.link_address = ++pc;
            } else {
                id_ex.link_address = pc;
            }

        } else if (opcode == SYSCALL){
            // setting control signals
            control_unit.setControlSignals(opcode);
            control_unit.forward_control_signals(ex_mem);
            id_ex.opcode = opcode;

        } else if (opcode != J && opcode!= JAL && opcode != R_TYPE && opcode != SYSCALL){
            ui32 rs = instruction >> 21 & 0x1F;
            ui32 rd = instruction >> 16 & 0x1F;
            i32 immediate = instruction & 0xFFFF;
            i32 sign_extended_imm = utils::sign_extend(immediate); // utility function to sign extend
            // if reg V0 (for syscall) just place straight into reg
            if (rd == V0){
                id_ex.syscall_code = sign_extended_imm;
            }
            // arguments for syscall
            switch (rd){
                case A0:
                    id_ex.arg1 = sign_extended_imm;
                    break;
                case A1:
                    id_ex.arg2 = sign_extended_imm;
                    break;
                case A2:
                    id_ex.arg3 = sign_extended_imm;
                    break;
                case A3:
                    id_ex.arg4 = sign_extended_imm;
                    break;
            }
            // adding to pipeline registers
            id_ex.opcode = opcode;
            id_ex.rs = registers[rs];
            id_ex.rs_reg = rs;
            id_ex.rd = registers[rd];
            id_ex.rd_reg = rd;
            id_ex.rt = 0;
            id_ex.rt_reg = 0;
            
            // data hazard load matching operand stall
            ex_mem.rs_copy = registers[rs];
            ex_mem.rd_copy = registers[rd];

            
            // detect stall condition ** Check if need here but think I do FIXME triggering when not supposed to
            if (id_ex.rs_reg != 0 || id_ex.rd_reg != 0) {
                if (control_unit.resolved == 0 && ex_mem.mem_read == 1 && id_ex.opcode != SYSCALL &&  // not when syscall because it would trigger when I didint want to like syscall before addi
                   (id_ex.rs_reg == ex_mem.destination_register || id_ex.rd_reg == ex_mem.destination_register)){
                    std::cout << "stall condition triggered" << std::endl;
                    control_unit.resolved = 1; // not resolved
                    mem_wb = {}; // might change to id_ex try it
                    control_unit.nop = 1;
                    if_id = {};
                    control_unit.reg_write = 1;
                }
            }

            // for branch
            if (opcode == BEQ || opcode == BGEZ || opcode == BGTZ || opcode == BLEZ || opcode == BLTZ){
                id_ex.branch_rd = rd;
                id_ex.branch_rs = rs;
                control_unit.branch = 1;
            }

            id_ex.immediate = sign_extended_imm;

            // setting control signals
            control_unit.setControlSignals(opcode);
            control_unit.forward_control_signals(ex_mem);
        }
    }

}


// execute for pipeline
void CPU::execute(){
    ui32 jump_address = 0;
    ui32 quotient = 0;
    ui32 remainder = 0;
    ex_mem.opcode = id_ex.opcode;
    ex_mem.rd = id_ex.rd; // c if its ok here might cause issues ** // forward destination reg ***add for all r type ins (if probalem add for each inst)
    if (ex_mem.alu_op == 10)
        ex_mem.destination_register = id_ex.rd;
    else 
        ex_mem.destination_register = id_ex.rd_reg;
    // data hazard (calc using value that has not been written back yet)
    // refactor into switch statement/function
    if (control_unit.forwardA == 1){
        id_ex.rs = mem_wb.mem_data_copy;
        control_unit.forwardA = 0;
    } else if (control_unit.forwardB == 1){
        id_ex.rt = mem_wb.mem_data_copy;
        control_unit.forwardB = 0;
    }

    if (control_unit.forwardA == 10){
        id_ex.rs = mem_wb.mem_data_copy1;
        control_unit.forwardA = 0;
    } else if (control_unit.forwardB == 10){
        id_ex.rt = mem_wb.mem_data_copy1;
        control_unit.forwardB = 0;
    }
   
    if (id_ex.opcode == R_TYPE){
        // |Opcode| |rs| |rt| |rd| |shamt| |opcode| Bits: (6,5,5,5,5,6)
        switch (id_ex.function_opcode){
            case ADD:
                ex_mem.alu_result = id_ex.rs + id_ex.rt;
                flags.setFlags(id_ex.rd,id_ex.rs,id_ex.rt);
                break;
            case SUB:
                ex_mem.alu_result = id_ex.rs - id_ex.rt;
                flags.setFlags(id_ex.rd,id_ex.rs,id_ex.rt);
                break;
            case MULT: // FIXME
                // rs * rt if rs*rt > 2^32 divide by 2 put half in low and half in high 
                ex_mem.alu_result = id_ex.rs * id_ex.rt; // mult rd rs rt
                if (ex_mem.alu_result > (1 << 31)){ // fix
                    ex_mem.store_val = ex_mem.alu_result / 2; // high 
                    ex_mem.store_val2 = ex_mem.alu_result / 2; // low
                }
                flags.setFlags(id_ex.rd,id_ex.rs,id_ex.rt);
                break;
            case DIV: 
                quotient = id_ex.rs / id_ex.rt;      // ** Put into a function and remove from here
                remainder = id_ex.rs % id_ex.rt;
                ex_mem.store_val = remainder; //  HI 
                ex_mem.store_val2 = quotient; //  LO
                flags.setFlags(id_ex.rs,id_ex.rt,id_ex.rd);
                break;
            case MFHI: // move from HI register
                ex_mem.store_val = special_registers[SpecialRegisters::HI]; 
                break;
            case MFLO: // move from LO register
                ex_mem.store_val = special_registers[SpecialRegisters::LO];
                break;
            case AND: // AND $rd, $rs, $rt
                ex_mem.alu_result = id_ex.rs & id_ex.rt; // bitwise AND 
                break;
            case OR: 
                ex_mem.alu_result = id_ex.rs | id_ex.rt;; //bitwise OR
                break;
            case XOR: 
                ex_mem.alu_result = id_ex.rs ^ id_ex.rt;; // bitwise XOR
                break;
            case NOR:
                ex_mem.alu_result = ~(id_ex.rs | id_ex.rt); // bitwise NOR
                break;
            case SLL: // SLL $rd, $rt, shamt  
                ex_mem.alu_result = id_ex.rt << id_ex.shamt; // Shift Left Logical: $rd = $rt << shamt
                break;
            case SRL: // SRL $rd, $rt, shamt
                ex_mem.alu_result = id_ex.rt >> id_ex.shamt;
                break;
            case JALR: // JALR $rs, $rd # Jump and Link Register: Jump to address in $rs, link to $rd
                ex_mem.link_address = ++pc; 
                pc = id_ex.rs; // set program counter to target address
                ex_mem.jump_address = pc;
                break;

        }
    } else { // i type and jump 
        switch (id_ex.opcode){
            case J:
                jump_address = utils::jumpOffsetPipeline(id_ex);
                ex_mem.link_address = jump_address;
                control_unit.pc_src = 1;
                break;
            // jump and save address of next instruction
            case JAL:
                jump_address = utils::jumpOffsetPipeline(id_ex);
                ex_mem.jump_address = jump_address;
                ex_mem.link_address = id_ex.link_address;
            case SYSCALL: 
                std::cout << "Executing Syscall" << std::endl; // debug
                // for pipeline but causing issues redo logic for syscalls in pipeline phase
                // registers[Registers::A0] = id_ex.arg1;
                registers[Registers::A1] = id_ex.arg2;
                registers[Registers::A2] = id_ex.arg3;
                registers[Registers::A3] = id_ex.arg4;
                registers[Registers::V0] = id_ex.syscall_code;
                executeSyscall();
            // update for pipeline
            case ADDI: // ADDI $rd, $rs, immediate 
                ex_mem.alu_result = id_ex.rs + id_ex.immediate;
                if (id_ex.rs_reg == SP)
                    registers[SP] += id_ex.immediate;
                flags.setFlags(id_ex.rs,id_ex.rd, id_ex.immediate);
                break;
            case ANDI: // (AND Immediate)
                ex_mem.alu_result = id_ex.rs & id_ex.immediate;
                break;
            case ORI:  // (OR Immediate)
                ex_mem.alu_result = id_ex.rs | id_ex.immediate;
                break;
            case XORI: // (XOR Immediate)
                ex_mem.alu_result = id_ex.rs ^ id_ex.immediate;
                break;
            case LW:   // (Load Word)
                ex_mem.alu_result = memory[id_ex.rs + id_ex.immediate];
                break;
            case SW:   // (Store Word)
                ex_mem.alu_result = id_ex.rs + id_ex.immediate;
                if (control_unit.data_hazard_mem_to_mem == 1){
                    ex_mem.store_val = mem_wb.mem_data_copy; // for load imm
                    // std::cout << "this should be 7: " << ex_mem.store_val << "\n"; debug
                    control_unit.data_hazard_mem_to_mem = 0;
                } else 
                    ex_mem.store_val = id_ex.rd;
                // memory[(registers[rs]) + sign_extended_imm] = registers[rd]; 
                break;
            case LB:   // (Load Byte) LB $rt, offset($rs)
                ex_mem.alu_result = id_ex.rs + id_ex.immediate;
                // registers[rd] = static_cast<ui8>(memory[rs+sign_extended_imm]);
                break;
            case LH:   // (Load Halfword)
                ex_mem.alu_result = id_ex.rs + id_ex.immediate;
                // registers[rd] = static_cast<ui16>(memory[rs+sign_extended_imm]);
                break;
            case SB:   // (Store Byte)
                ex_mem.alu_result = id_ex.rs + id_ex.immediate;
                if (control_unit.data_hazard_mem_to_mem == 1){
                    ex_mem.store_val = mem_wb.mem_data_copy;
                } else 
                    ex_mem.store_val = id_ex.rd;
                // memory[rs+sign_extended_imm] = static_cast<ui8>(registers[rd]);
                break;
            case SH:   // (Store Halfword)
                ex_mem.alu_result = id_ex.rs + id_ex.immediate;
                if (control_unit.data_hazard_mem_to_mem == 1){
                    ex_mem.store_val = mem_wb.mem_data_copy;
                    // std::cout << "this should be 7: " << "\n"; debug
                    control_unit.data_hazard_mem_to_mem = 0;
                } else 
                    ex_mem.store_val = id_ex.rd;
                // memory[rs+sign_extended_imm] = static_cast<ui16>(registers[rd]);
                break;
            case BEQ:  // BEQ $rs, $rt, offset
                if (registers[id_ex.rs] == id_ex.branch_rd || registers[id_ex.rd] == id_ex.branch_rs){ // need to check but should work
                    ex_mem.link_address = id_ex.link_address + id_ex.immediate; // forward new pc (change)
                    control_unit.pc_src = 1;
                }
                    // pc = ex_mem.link_address;
                    // special_registers[SpecialRegisters::PC] = pc;
                break;
            case BNE:  // BNE (Branch if Not Equal)
                // sign_extended_imm <<= 2;
                if (id_ex.rs != id_ex.rd)
                    ex_mem.link_address = id_ex.link_address + id_ex.immediate; // forward new PC
                    control_unit.pc_src = 1;
                    // special_registers[SpecialRegisters::PC] = pc;
                break;
            case BGEZ: // BGEZ $t0, LABEL
                // sign_extended_imm <<= 2;
                if (id_ex.rs >= 0)
                    ex_mem.link_address = id_ex.link_address + id_ex.immediate; // forward new PC
                    control_unit.pc_src = 1;
                    // special_registers[SpecialRegisters::PC] = pc; 
                break;
            case BLTZ: // BLTZ (Branch if Less Than Zero)
                // sign_extended_imm <<= 2;
                if (id_ex.rs < 0)
                    ex_mem.link_address = id_ex.link_address + id_ex.immediate;
                    control_unit.pc_src = 1;
                    // special_registers[SpecialRegisters::PC] = pc;
                break;
            case BGTZ: // BGTZ (Branch if Greater Than Zero)
                // sign_extended_imm <<= 2;
                if (id_ex.rs >  0)
                    ex_mem.link_address = id_ex.link_address + id_ex.immediate;
                    control_unit.pc_src = 1;
                    // special_registers[SpecialRegisters::PC] = pc;
                break;
            case BLEZ: // BLEZ (Branch if Less Than or Equal to Zero)
                // sign_extended_imm <<= 2;
                if (id_ex.rs <= 0)
                    ex_mem.link_address = id_ex.link_address + id_ex.immediate;
                    control_unit.pc_src = 1;
                    // special_registers[SpecialRegisters::PC] = pc;
                break;
            case LUI:  // LUI (Load Upper Immediate)
                ex_mem.alu_result = (id_ex.immediate << 16); // shift imm to upper 16 bits
                break;
            case LI: // load immediate
                ex_mem.alu_result = id_ex.immediate;
                break;
        }
    }

    //******data hazard (calculations using a value that has not been written back yet)******
    // if (ex_mem.reg_write == 1 && ex_mem.destination_register != 0 && ex_mem.destination_register == id_ex.rs_reg){
    //         control_unit.forwardA = 10;
    //         ex_mem.rs_copy = ex_mem.alu_result;
    //         std::cout << "data hazard triggerd1" << std::endl; //debug
    //         std::cout << "ex_mem_rs_copy: " << ex_mem.rs_copy << std::endl; //debug
    //     }
    // else if (ex_mem.reg_write == 1 && ex_mem.destination_register != 0 && ex_mem.destination_register == id_ex.rt_reg){
    //     control_unit.forwardB = 10;
    //     ex_mem.rt_copy = ex_mem.alu_result;
    //     std::cout << "data hazard triggerd2" << std::endl; //debug
    //     std::cout << "ex_mem_rd_copy: " << ex_mem.rd_copy << std::endl; //debug
    // }
    //***************************************************************************************
    // forward control signals from id_ex to ex_mem 
    control_unit.forward_control_signals_mem_wb(ex_mem,mem_wb);
}

// read or write from memory cache
// ************add syscall execute functionality****************
void CPU::mem(){
    // forward to final pipeline register stage
    mem_wb.opcode = ex_mem.opcode;
    mem_wb.destination_register = ex_mem.destination_register;
    mem_wb.alu_result = ex_mem.alu_result;
    mem_wb.store_val = ex_mem.store_val;
    mem_wb.store_val2 = ex_mem.store_val2;
    // mem_wb.mem_data_copy = memory[ex_mem.alu_result]; // maybe can remove
    id_ex.mem_wb_alu_result = mem_wb.alu_result; // forward for data hazard 
    // std::cout << "mem_wb.alu: " << mem_wb.alu_result << std::endl;
    mem_wb.reg_write = ex_mem.reg_write;
    mem_wb.mem_to_reg = ex_mem.mem_to_reg;

    //******data hazard (calculations using a value that has not been written back yet)******
    if (mem_wb.reg_write == 1 && mem_wb.destination_register != 0 && ex_mem.rd != id_ex.rs_reg && 
       mem_wb.destination_register == id_ex.rs_reg){ // FIXME issue here the condition is not right for some reason
        control_unit.forwardA = 1;
        mem_wb.mem_data_copy = mem_wb.alu_result;
        std::cout << "data hazard triggerd mem rs" << std::endl; //debug
    }
    else if (mem_wb.reg_write == 1 && mem_wb.destination_register != 0 && ex_mem.rd != id_ex.rt_reg && 
        mem_wb.destination_register == id_ex.rt_reg){
         control_unit.forwardB = 1;
         mem_wb.mem_data_copy = mem_wb.alu_result;
         std::cout << "data hazard triggerd mem rt" << std::endl; //debug
     }
    //***************************************************************************************

    // mem to mem data hazard: if load immediately followed by a store
    if ((ex_mem.opcode == LW || ex_mem.opcode == LB || ex_mem.opcode == LH || ex_mem.opcode == LUI || ex_mem.opcode == LI) && 
        (id_ex.opcode == SW || id_ex.opcode == SB || id_ex.opcode == SH) || ((if_id.instruction >> 26) == SW || (if_id.instruction >> 26) == SB || (if_id.instruction >> 26) == SH)){
            control_unit.data_hazard_mem_to_mem = 1;
            wb_display.dont_write = 1;
            std::cout << "data hazard triggerd" << std::endl; //debug
        }

    // *** go over ***
    // branch pipeline hazard solution
    if (control_unit.branch == 1){
        id_ex.branch_rd = mem_wb.alu_result; // Check solution (Solved Problem: wanted value from second instruction has not been written back yet to registers in time for branch comparison) //**probably will need to change not a good solution
        id_ex.branch_rs = mem_wb.alu_result; 
        control_unit.branch = 0;
    }

    
    // mem stage main execution 
    if (mem_wb.mem_read == 1){
        mem_wb.mem_data = memory[ex_mem.alu_result];
    } else if (mem_wb.mem_write == 1){
        memory[ex_mem.alu_result] = mem_wb.store_val; 
    } 

    // check this (mem to mem hazard i.e. load followed by store)
    if ((mem_wb.opcode == LI || mem_wb.opcode == LB || mem_wb.opcode == LH || mem_wb.opcode == LUI || 
        mem_wb.opcode == LW) && control_unit.data_hazard_mem_to_mem == 1){ 
        mem_wb.mem_data_copy = ex_mem.alu_result; // for imm
        std::cout << "worked" << "\n";
    }

}

// write back to registers
void CPU::WB(){
    // Take result from memory not alu and store in reg
    wb_display.opcode = mem_wb.opcode; // forward for displaying final stage of pipeline
    // std::cout << "mem_wb alu result: " <<  mem_wb.alu_result << std::endl;
    // if stall reset control signal to 1
    if (control_unit.nop == 1){
        mem_wb.reg_write = 1;
    }
    if (mem_wb.reg_write == 1 && mem_wb.mem_to_reg == 1){
        registers[mem_wb.destination_register] = mem_wb.mem_data;
        
    } else if (mem_wb.reg_write == 1 && mem_wb.mem_to_reg == 0 && wb_display.dont_write == 0){
        registers[mem_wb.destination_register] = mem_wb.alu_result;
    } 

    if (wb_display.opcode == LI && wb_display.dont_write == 1){
        wb_display.dont_write == 0;
    } 

    // check if stall is resolved to free pipeline at decode and fetch stages (currently frozen)
    if (control_unit.nop == 1 && (wb_display.opcode == LI || wb_display.opcode == LUI || wb_display.opcode == LH || 
        wb_display.opcode == LW || wb_display.opcode == LB || wb_display.opcode == ADD)){ // might need to use mem_wb opcode instead
        control_unit.nop = 0;
        control_unit.resolved = 0;
        control_unit.redo_decode = 1;
        std::cout << "stall resolved" << std::endl;
    } 


    //******data hazard (calculations using a value that has not been written back yet)******
    if (mem_wb.reg_write == 1 && mem_wb.destination_register != 0 && mem_wb.destination_register != id_ex.rs_reg && 
        mem_wb.destination_register == id_ex.rs_reg){
            control_unit.forwardA = 10;
            mem_wb.mem_data_copy1 = mem_wb.alu_result;
            std::cout << "data hazard triggerd wb rs" << std::endl; //debug
    }
    else if (mem_wb.reg_write == 1 && mem_wb.destination_register != 0 && ex_mem.rd != id_ex.rt_reg && 
        mem_wb.destination_register == id_ex.rt_reg){
        control_unit.forwardB = 10;
        mem_wb.mem_data_copy1 = mem_wb.alu_result;
        std::cout << "data hazard triggerd wb rt" << std::endl; //debug
        std::cout << "mem wb copy data 1: " << mem_wb.mem_data_copy1 << std::endl; //debug
    }
    //***************************************************************************************

}

/****************************************
* Check what type of instruction it is according to opcode 
* 0000 r type not that or specific jump instruction (only have 3)
* after that extract correct registers or imm for execution
* do necessary calculations and set necessary flags
* normal not pipelined version
****************************************/
bool CPU::decodeExecute(ui32 instruction){
    bool successfull = true;
    
    // extract instruction opcode (last 6 bits)
    ui32 opcode = instruction >> 26; 
    
    // Select Instruction Type (R, I ,J)
    // R Type Instructions
    // |Opcode| |rs| |rt| |rd| |shamt| |opcode| Bits: (6,5,5,5,5,6)
    if (opcode == R_TYPE){
        ui32 function_opcode = instruction & 0x3F; // extract function code from first 6 bits
        ui32 rs = instruction >> 21 & 0x1F; // source register
        ui32 rt = instruction >> 16 & 0x1F;// target register
        ui32 rd = instruction >> 11 & 0x1F; // destination register
        ui32 shamt = instruction >> 6 & 0x3F;  // shift amount
        // Handle R-type instructions based on function opcode
        switch (function_opcode){
            case ADD:
                registers[rd] = registers[rs] + registers[rt]; // add rd rs rt
                flags.setFlags(rd,rs,rt); // set flags
                break;
            case SUB:
                registers[rd] = registers[rs] - registers[rt];  // sub rd rs rt
                flags.setFlags(rd,rs,rt); // set flags 
                break;
            case MULT: 
                // rs * rt if rs*rt > 2^32 divide by 2 put half in low and half in high 
                registers[rd] = registers[rs] * registers[rt]; // mult rd rs rt
                flags.setFlags(rd,rs,rt);
                break;
            case DIV: 
                if (registers[rt] == 0) // check for division by 0  ** Change and just setflags
                    flags.setExceptionFlag(true);
                else {
                    ui32 quotient = registers[rs] / registers[rt];                          // ** Put into a function and remove from here
                    ui32 remainder = registers[rs] % registers[rt];
                    special_registers[SpecialRegisters::HI] = remainder; // remainder stored in HI 
                    special_registers[SpecialRegisters::LO] = quotient; // quotient stored in LO
                    registers[rd] = special_registers[SpecialRegisters::LO];
                    flags.setFlags(rs,rt,rd);
                }
                break;
            case MFHI: // move from HI register
                registers[rd] = special_registers[SpecialRegisters::HI]; 
                break;
            case MFLO: // move from LO register
                registers[rd] = special_registers[SpecialRegisters::LO];
                break;
            case AND: // AND $rd, $rs, $rt
                registers[rd] = registers[rs] & registers[rt]; // bitwise AND 
                break;
            case OR: 
                registers[rd] = registers[rs] | registers[rt]; //bitwise OR
                break;
            case XOR: 
                registers[rd] = registers[rs] ^ registers[rt]; // bitwise XOR
                break;
            case NOR:
                registers[rd] = ~(registers[rs] | registers[rt]); // bitwise NOR
                break;
            case SLL: // SLL $rd, $rt, shamt  
                registers[rd] = registers[rt] << shamt; // Shift Left Logical: $rd = $rt << shamt
                break;
            case SRL: // SRL $rd, $rt, shamt
                registers[rd] = registers[rt] >> shamt;
                break;
            case JALR: // JALR $rs, $rd # Jump and Link Register: Jump to address in $rs, link to $rd
                registers[rd] = ++pc; 
                pc = registers[rs]; // set program counter to target address
                special_registers[SpecialRegisters::PC] = pc;
                break;

        }
    }
    // Jump Instructions
    // // | Opcode (6 bits) | Address (26 bits) |
    else if (opcode == J){ // J target
        utils::jumpOffset(instruction,pc); // set pc to target address
    }
    else if (opcode == JAL){
        registers[Registers::RA] = ++pc; // save address of next instruction (to return to)
        utils::jumpOffset(instruction,pc); // set pc to target address
    }else if (opcode == SYSCALL){
        std::cout <<"executing syscall" << std::endl;
        executeSyscall(); // execute syscalls
    }
    
    // I Type Instructions
    // |Opcode| |rs| |rd| |Immediate| Bits: (6,5,5,16)
    else if (opcode != J && opcode!= JAL && opcode != R_TYPE && opcode != SYSCALL){
        ui32 rs = instruction >> 21 & 0x1F;
        ui32 rd = instruction >> 16 & 0x1F;
        i32 immediate = instruction & 0xFFFF;
        i32 sign_extended_imm = utils::sign_extend(immediate); // utility function to sign extend
        switch(opcode){
            case ADDI: // ADDI $rd, $rs, immediate 
                registers[rd] = registers[rs] + sign_extended_imm;
                if (registers[rs] == SP)
                    registers[SP] += sign_extended_imm;
                flags.setFlags(rs,rd, sign_extended_imm);
                break;
            case ANDI: // (AND Immediate)
                registers[rd] = registers[rs] & sign_extended_imm;
                break;
            case ORI:  // (OR Immediate)
                registers[rd] = registers[rs] | sign_extended_imm;
                break;
            case XORI: // (XOR Immediate)
                registers[rd] = registers[rs] ^ sign_extended_imm;
                break;
            case LW:   // (Load Word)
                registers[rd] = memory[registers[rs] + sign_extended_imm];
                break;
            case SW:   // (Store Word)
                memory[(registers[rs]) + sign_extended_imm] = registers[rd]; 
                break;
            case LB:   // (Load Byte) LB $rt, offset($rs)
                registers[rd] = static_cast<ui8>(memory[rs+sign_extended_imm]);
                break;
            case LH:   // (Load Halfword)
                registers[rd] = static_cast<ui16>(memory[rs+sign_extended_imm]);
                break;
            case SB:   // (Store Byte)
                memory[rs+sign_extended_imm] = static_cast<ui8>(registers[rd]);
                break;
            case SH:   // (Store Halfword)
                memory[rs+sign_extended_imm] = static_cast<ui16>(registers[rd]);
                break;
            case BEQ:  // BEQ $rs, $rt, offset
                // sign_extended_imm <<= 2; // shfit left by 2 to get correct address(32 bit mem so 4 bytes per mem location (2^2))
                if (registers[rs] == registers[rd])
                    pc += sign_extended_imm;
                    special_registers[SpecialRegisters::PC] = pc;
                break;
            case BNE:  // BNE (Branch if Not Equal)
                sign_extended_imm <<= 2;
                if (registers[rs] != registers[rd])
                    pc += sign_extended_imm;
                    special_registers[SpecialRegisters::PC] = pc;
                break;
            case BGEZ: // BGEZ $t0, LABEL
                sign_extended_imm <<= 2;
                if (registers[rs] >= 0)
                    pc += sign_extended_imm;
                    special_registers[SpecialRegisters::PC] = pc; 
                break;
            case BLTZ: // BLTZ (Branch if Less Than Zero)
                sign_extended_imm <<= 2;
                if (registers[rs] < 0)
                    pc += sign_extended_imm;
                    special_registers[SpecialRegisters::PC] = pc;
                break;
            case BGTZ: // BGTZ (Branch if Greater Than Zero)
                sign_extended_imm <<= 2;
                if (registers[rs] > 0)
                    pc += sign_extended_imm;
                    special_registers[SpecialRegisters::PC] = pc;
                break;
            case BLEZ: // BLEZ (Branch if Less Than or Equal to Zero)
                sign_extended_imm <<= 2;
                if (registers[rs] <= 0)
                    pc += sign_extended_imm;
                    special_registers[SpecialRegisters::PC] = pc;
                break;
            case LUI:  // LUI (Load Upper Immediate)
                registers[rd] = (immediate << 16); // shift imm to upper 16 bits
                break;
            case LI: // load immediate
                registers[rd] = sign_extended_imm;
                break;

        }
    }

    return successfull;
    
} 

// syscall for pipelined cpu * just switches out A0 reg for ex_mem.arg1 (might be able to remove)
void CPU::executeSyscall_pipeline(){
    ui32 syscall_val = id_ex.syscall_code; // get syscall val from reg V0
    switch(syscall_val){
        case PRINT_INT:
            std::cout << ex_mem.arg1 << "\n";
            break;
        case PRINT_FLOAT:
            std::cout << static_cast<float>(fp_registers[FloatingPointRegisters::F12]) << "\n";
            break;
        case PRINT_DOUBLE:
            std::cout << static_cast<double>(fp_registers[FloatingPointRegisters::F12]) << "\n";
            break;
        case PRINT_STRING:
            std::cout << reinterpret_cast<const char*>(&memory[ex_mem.arg1]) << "\n";
            break;
        case READ_INT:
            i32 val;
            std::cin >> val;
            registers[ex_mem.syscall_code] = val;
            break;
        case READ_FLOAT: {
            float val;
            std::cin >> val;
            fp_registers[FloatingPointRegisters::F0] = val;
            break;
        }
        case READ_DOUBLE: {
            double val ;
            std::cin >> val;
            fp_registers[FloatingPointRegisters::F0] = val;
            break;
        }
        case READ_STRING: // go over method
            utils::syscall_read_string(memory,registers);
            break;
        case EXIT: {
            halted = true;
            break;
        }
        case EXIT_STATUS: {
            ui32 exit_status = ex_mem.arg1;
            std::cerr << "Program exited with status: " << exit_status << "\n";
            halted = true;
            break;
        }
        case OPEN_FILE:{
            utils::syscall_open_file(memory, registers,file_descriptors);
            break;
        }
        case READ_FILE:
            utils::syscall_read_file(memory, registers, file_descriptors);
            break;
        case WRITE_FILE:
            utils::syscall_write_file(memory, registers, file_descriptors);
            break;
        case CLOSE_FILE:
            utils::syscall_close_file(file_descriptors);
            break;
        case EXIT_WITH_STATUS:
            utils::syscall_exit_status(registers, halted);
            break;
    }
}

// execute syscalls according to syscall in val register and then use the correct arg register to execute
// const ui8 val, const ui8 arg
void CPU::executeSyscall(){
    ui32 syscall_val = registers[Registers::V0]; // get syscall val from reg V0
    switch(syscall_val){
        case PRINT_INT:
            std::cout << registers[Registers::A0] << "\n";
            break;
        case PRINT_FLOAT:
            std::cout << static_cast<float>(fp_registers[FloatingPointRegisters::F12]) << "\n";
            break;
        case PRINT_DOUBLE:
            std::cout << static_cast<double>(fp_registers[FloatingPointRegisters::F12]) << "\n";
            break;
        case PRINT_STRING:
            std::cout << reinterpret_cast<const char*>(&memory[registers[Registers::A0]]) << "\n";
            break;
        case READ_INT:
            i32 val;
            std::cin >> val;
            registers[Registers::V0] = val;
            break;
        case READ_FLOAT: {
            float val;
            std::cin >> val;
            fp_registers[FloatingPointRegisters::F0] = val;
            break;
        }
        case READ_DOUBLE: {
            double val ;
            std::cin >> val;
            fp_registers[FloatingPointRegisters::F0] = val;
            break;
        }
        case READ_STRING: // go over method
            utils::syscall_read_string(memory,registers);
            break;
        case EXIT: {
            halted = true;
            break;
        }
        case EXIT_STATUS: { // need to fix this 
            ui32 exit_status = special_registers[SpecialRegisters::EPC];
            std::cerr << "Program exited with status: " <<  exit_status << "\n";
            halted = true;
            break;
        }
        case OPEN_FILE:{
            utils::syscall_open_file(memory, registers,file_descriptors);
            break;
        }
        case READ_FILE:
            utils::syscall_read_file(memory, registers, file_descriptors);
            break;
        case WRITE_FILE:
            utils::syscall_write_file(memory, registers, file_descriptors);
            break;
        case CLOSE_FILE:
            utils::syscall_close_file(file_descriptors);
            break;
        case EXIT_WITH_STATUS:
            utils::syscall_exit_status(registers, halted);
            break;
    }
}


// destructor
CPU::~CPU(){
    delete[] memory; //  clean up memory
}








