#include "virtual_cpu.h"

// Constructor
CPU::CPU(): memory(new ui32[MEMORY_SIZE]()), registers{0}, special_registers{0},
            fp_registers{0}, sp(STACK_MEMORY), gp(GLOBAL_DATA_MEMORY),pc(0), halted(false) {
    // Initializes Memory to 1GB, registers to 0, the sp and gp to respective memory locations,
    // pc to 0 and halted to false
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
}

// Fetch Instruction from memory and pass to decodeExecute
void CPU::fetch(){
    ui32 instruction = memory[pc];
    pc++;
    decodeExecute(instruction);
}

/****************************************
* Check what type of instruction it is according to opcode 
* 0000 r type not that or specific jump instruction (only have 3)
* after that extract correct registers or imm for execution
* do necessary calculations and set necessary flags
****************************************/
bool CPU::decodeExecute(ui32 instruction){

    // extract instruction opcode (last 6 bits)
    ui8 opcode = instruction >> 26; 

    // Select Instruction Type (R, I ,J)
    switch(opcode){
        
        // R Type Instructions
        // |Opcode| |rs| |rt| |rd| |shamt| |opcode| Bits: (6,5,5,5,5,6)
        case R_TYPE: 
            ui8 function_opcode = instruction & 0x3F; // extract function code from first 6 bits
            ui8 rs =  instruction >> 21 & 0x1F; // source register
            ui8 rt = instruction >> 16 & 0x1F; // target register
            ui8 rd = instruction >> 11 & 0x1F; // destination register
            ui8 shamt = instruction >> 6 & 0x1F;  // shift amount

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
                    registers[rd] = registers[rs] * registers[rt]; // mult rd rs rt
                    flags.setFlags(rd,rs,rt);
                    break;
                case DIV: 
                    if (registers[rt] == 0) // check for division by 0 
                        flags.setExceptionFlag(true);
                    else {
                        ui32 quotient = registers[rs] / registers[rt];
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
                case JALR: // JALR $rd, $rs # Jump and Link Register: Jump to address in $rs, link to $rd
                    registers[rd] = ++pc; 
                    pc = registers[rs]; // set program counter to target address
                    break;

            }
            break;  // Exit the R-type case
        
        // Jump Instructions
        // // | Opcode (6 bits) | Address (26 bits) |
        case J: // J target
            utils::jumpOffset(instruction,pc); // set pc to target address
            break;
        case JAL:
            registers[Registers::RA] = ++pc; // save address of next instruction (to return to)
            utils::jumpOffset(instruction,pc); // set pc to target address
            break;
        
        // Syscalls
        case SYSCALL:
            executeSyscall(); // execute syscalls
            break;

        // I Type Instructions
        // |Opcode| |rs| |rd| |Immediate| Bits: (6,5,5,16)
        default: 
            ui8 rs = instruction >> 21 & 0x1F;
            ui8 rd = instruction >> 16 & 0x1F;
            ui16 immediate = instruction & 0xFFFF;
            ui32 sign_extended_imm = utils::sign_extend(immediate); // utility function to sign extend
            case ADDI: // ADDI $rd, $rs, immediate 
                registers[rd] = registers[rs] + sign_extended_imm;
                flags.setFlags(rs,rd, sign_extended_imm);
                break;
            case ANDI: // (AND Immediate)
                registers[rd] = registers[rs] & sign_extended_imm;
                break;
            case ORI:  // (OR Immediate)
                registers[rd] = registers[rs] | sign_extended_imm;
                // Handle ORI
                break;
            case XORI: // (XOR Immediate)
                registers[rd] = registers[rs] ^ sign_extended_imm;
                break;
            case LW:   // (Load Word)
                registers[rd] = memory[rs + sign_extended_imm];
                break;
            case SW:   // (Store Word)
                memory[rs + sign_extended_imm] = registers[rd]; 
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
                sign_extended_imm <<= 2; // shfit left by 2 to get correct address(32 bit mem so 4 bytes per mem location (2^2))
                if (registers[rs] == registers[rt])
                    pc += sign_extended_imm;
                break;
            case BNE:  // BNE (Branch if Not Equal)
                sign_extended_imm <<= 2;
                if (registers[rs] != registers[rt])
                    pc += sign_extended_imm;
                break;
            case BGEZ: // BGEZ $t0, LABEL
                sign_extended_imm <<= 2;
                if (registers[rs] >= 0)
                    pc += sign_extended_imm;
                break;
            case BLTZ: // BLTZ (Branch if Less Than Zero)
                sign_extended_imm <<= 2;
                if (registers[rs] < 0)
                    pc += sign_extended_imm;
                break;
            case BGTZ: // BGTZ (Branch if Greater Than Zero)
                sign_extended_imm <<= 2;
                if (registers[rs] > 0)
                    pc += sign_extended_imm;
                break;
            case BLEZ: // BLEZ (Branch if Less Than or Equal to Zero)
                sign_extended_imm <<= 2;
                if (registers[rs] <= 0)
                    pc += sign_extended_imm;
                break;
            case LUI:  // LUI (Load Upper Immediate)
                registers[rd] = (immediate << 16); // shift imm to upper 16 bits
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
            std::cout << registers[Registers::A0] << "\n";
            break;
        case READ_INT:
            i32 val;
            std::cin >> val;
            registers[Registers::V0] = val;
            break;
        case READ_FLOAT:
            float val;
            std::cin >> val;
            fp_registers[FloatingPointRegisters::F0] = val;
            break;
        case READ_DOUBLE:
            double val ;
            std::cin >> val;
            fp_registers[FloatingPointRegisters::F0] = val;
            break;
        case READ_STRING: // go over method
            utils::syscall_read_string(memory,registers);
            break;
        case EXIT:
            halted = true;
            break;
        case EXIT_STATUS:
            ui32 exit_status = registers[Registers::A0];
            std::cerr << "Program exited with status: " << exit_status << "\n";
            halted = true;
            break;
        case OPEN_FILE:
            utils::syscall_open_file(memory, registers,file_descriptors);
            break;
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








