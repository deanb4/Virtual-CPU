#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include "registers.h"
#include "syscalls.h"
#include "virtual_cpu.h"
#include "pipeline_registers.h"
// had to include for carter
#include <iostream>
#include <string>
#include <unordered_map>
#include <random>
#include <fstream>
// ******************
#define ui32 uint32_t
#define i32 int32_t
#define ui16 uint16_t

namespace utils{

    // check for integer overflow
    bool checkIntOverflow(const ui32 r1, const ui32 r2, const ui32 r3){
        // check if r2 & r3 have same sign and if the result r1 has a different sign from operands
        return ((r2 & 0x80000000) == (r3 & 0x80000000)) && ((r1 & 0x80000000) != (r2 & 0x80000000));
    
    }

    // used for J and JAL to get target pc
    void jumpOffset(ui32 instruction, ui32& pc){
        ui32 target = instruction & 0x03FFFFFF; // mask lower 26 bits
        // target <<= 2; // shift target address by 2 (check doesnt make sense)
        ui32 upper_pc = pc & 0xF0000000; // get upper 4 bits from pc. Ensures that jumps remain within the same 256MB region of memory
        ui32 jump_address = upper_pc | target; // form full address
        pc = jump_address;

    }

    ui32 jumpOffsetPipeline(ID_EX id_ex){
        ui32 target = id_ex.instruction & 0x03FFFFFF; // mask lower 26 bits
        // check for shifting amt
        ui32 upper_pc = id_ex.link_address & 0xF0000000; // get upper 4 bits from pc. Ensures that jumps remain within the same 256MB region of memory
        ui32 jump_address = upper_pc | target; // form full address
        return jump_address;
    }

    // sign extend utility function 
    ui32 sign_extend(const ui16& imm){
        ui32 sign_extended_imm = imm;
        if (imm & 0x8000) // if negative (16th bit 1)
            sign_extended_imm |= 0xFFFF0000; // trailing 1's (-)
        else 
            sign_extended_imm &= 0x0000FFFF; // sign extend trailing 0's (+)
        
        return sign_extended_imm;
    }

    // read string syscall (test and change)
    void syscall_read_string(ui32* mem, ui32* reg){
        std::cin.ignore();
        std::string s;
        std::getline(std::cin, s);
        ui32 len = s.size();
        ui32 mem_space = std::ceil(len/4);
        ui32 address = 0x00010000;
        // get to empty mem address in data section
        while (mem[address]){ // add bounds checking
            address++;
        }
        reg[Registers::A0] = address;
        ui32 j = 0;
        for (ui32 i = 0; i <= mem_space; i++){
            if (i % 4 == 0)
                address+=j;
                j++;
            mem[address] = s[i];
        }
        mem[address+1] = '\0';
    }

    void syscall_open_file(ui32* mem, ui32* reg, std::unordered_map<i32,std::string> &file_desript){
        const char* file_path = reinterpret_cast<const char*>(mem[reg[Registers::A0]]); // get file path
        ui32 flags = reg[Registers::A1]; // get flags
        std::random_device rd;
        std::uniform_int_distribution<> dist(3,1024); // random val between 3-1024
        i32 file_descriptor = dist(rd); // get random file descriptor val
        // open file bases on flags
        std::ios_base::openmode mode = std::ios::in; // default to reading
        if (flags == FileAccessFlags::RDONLY)
            mode = std::ios::in;
        else if (flags == FileAccessFlags::WRONLY)
            mode = std::ios::out;
        else if (flags == FileAccessFlags::CREAT)
            mode = std::ios::out | std::ios::trunc;
        try {
            std::fstream file(file_path, mode);
            if (file.is_open()){
                reg[Registers::V0] = file_descriptor;
                file_desript[file_descriptor] = std::string(file_path);
                file.close();
            } else 
                reg[Registers::V0] = -1;

        } catch (const std::exception& e){ // set exception flag****
            std::cerr << "Error Opening file: " << e.what() << "\n";
            reg[Registers::V0] = -1; 
        }
    }

    // | **14**    | Read File   | File descriptor | Buffer address | Number of bytes | - | Bytes read |
    // *** Go over this as proabably buggy or logic error
    void syscall_read_file(ui32* mem, ui32 *reg, std::unordered_map<i32,std::string> &file_desript) {
        if (reg[Registers::V0]){
            std::string file_path = file_desript[Registers::A0]; 
            std::fstream file(file_path);
            ui32 bytes_read = file.gcount(); // get number of bytes read
            reg[Registers::V0] = bytes_read; // store bytes read in register V0
            if (file.is_open()){
                std::string buffer;
                file.read(&buffer[0],buffer.size()); // read content into buffer
                std::cout << "File Succesfully Opened: " << buffer << "\n"; // display content
                reg[Registers::A2] = buffer.size(); // store string length in A2
                ui32 address = 0x00010000; // find first empty spot in data segment of memory
                i32 i = 0;
                while (mem[address]){ // must fix and add bounds checking*** 
                    address+=1;
                    i++;
                }
                reg[Registers::T0] = i; // offset from start of data segment in memory
                std::copy(buffer.begin(), buffer.end(), reinterpret_cast<ui32*>(mem) + address); // copy from buffer to memory
                file.close();
            } else {
                std::cerr << "Error: File is not open. " << "\n";
                reg[Registers::V0] = -1; // set exception flag
            }
        } else{
            std::cerr << "Error: Nothing to read from. " << "\n";
        }
    }

    // write to file 
    void syscall_write_file(ui32* mem, ui32 *reg, std::unordered_map<i32,std::string> &file_desript){
        std::string file_path = file_desript[Registers::A0]; 
        std::fstream file(file_path, std::ios::out);
        ui32 offset = reg[Registers::T0];
        i32 address = 0x00010000 + offset;
        char* buffer = reinterpret_cast<char*>(address);
        i32 bytes_to_write = reg[Registers::A2];
        if(file.is_open()){
            file.write(buffer, bytes_to_write);

            if (file.good()){
                reg[Registers::V0] == bytes_to_write;

            } else {
                std::cerr << "Error writing to the file. " << "\n"; 
                reg[Registers::V0] = -1;
            }

        } else {
            std::cerr << "Error! Can not find file. " << "\n";
            reg[Registers::V0] = -1;
        }
        file.close();
    }


    // close file
    void syscall_close_file(std::unordered_map<i32,std::string> &file_desript){
        std::string file_path = file_desript[Registers::A0]; 
        std::fstream file(file_path);
        if (file.is_open())
            file.close();

    }

    void syscall_exit_status(ui32* reg, bool halt){
        ui32 exit_status = reg[Registers::A0];
        std::cout << "Exiting program with status: " << exit_status << "\n";
        halt = true;
        reg[Registers::V0] = exit_status;

    }

    // util function to display registers
    std::string get_general_register(ui32 reg){
        std::unordered_map<int, std::string> regToStr {
            {ZERO, "$zero"},
            {AT, "$at"},
            {V0, "$v0"},
            {V1, "$v1"},
            {A0, "$a0"},
            {A1, "$a1"},
            {A2, "$a2"},
            {A3, "$a3"},
            {T0, "$t0"},
            {T1, "$t1"},
            {T2, "$t2"},
            {T3, "$t3"},
            {T4, "$t4"},
            {T5, "$t5"},
            {T6, "$t6"},
            {T7, "$t7"},
            {S0, "$s0"},
            {S1, "$s1"},
            {S2, "$s2"},
            {S3, "$s3"},
            {S4, "$s4"},
            {S5, "$s5"},
            {S6, "$s6"},
            {S7, "$s7"},
            {T8, "$t8"},
            {T9, "$t9"},
            {K0, "$k0"},
            {K1, "$k1"},
            {GP, "$gp"},
            {SP, "$sp"},
            {FP, "$fp"},
            {RA, "$ra"}
        };
        
         // display registers
        return regToStr[reg];
        
    }

    std::string get_opcode(const uint8_t hex_opcode){
        std::string ret_opcode = "";
        std::unordered_map<uint8_t, std::string> opcodes{
            {0x20, "ADD"},
            {0x22, "SUB"},
            {0x18, "MULT"},
            {0x1A, "DIV"},
            {0x10, "MFHI"},
            {0x12, "MFLO"},
            {0x24, "AND"},
            {0x25, "OR"},
            {0x26, "XOR"},
            {0x27, "NOR"},
            {0x00, "SLL"},
            {0x02, "SRL"},
            {0x09, "JALR"},
            {0x08, "ADDI"},
            {0x0C, "ANDI"},
            {0x0D, "ORI"},
            {0x0E, "XORI"},
            {0x23, "LW"},
            {0x2B, "SW"},
            {0x11, "LB"},
            {0x21, "LH"},
            {0x28, "SB"},
            {0x29, "SH"},
            {0x04, "BEQ"},
            {0x05, "BNE"},
            {0x01, "BGEZ"},
            {0x07, "BGTZ"},
            {0x06, "BLEZ"},
            {0x3C, "BLTZ"},
            {0x0F, "LUI"},
            {0x39, "LI"},
            {0x33, "J"},
            {0x03, "JAL"},
            {0x0B, "JR"},
            {0x3F, "SYSCALL"}
        };

        if (opcodes.find(hex_opcode) == opcodes.end()){
            std::cerr << "Invalid Opcode" << std::endl;
            ret_opcode = "Unkown";

        } else if (hex_opcode == 0x00){
            ret_opcode = "0";
        
        } else
            ret_opcode = opcodes[hex_opcode];
        
        return ret_opcode;

    }



};

#endif
    