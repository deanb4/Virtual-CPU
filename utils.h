#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

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
        target <<= 2; // shift target address by 2
        ui32 upper_pc = pc & 0xF0000000; // get upper 4 bits from pc. Ensures that jumps remain within the same 256MB region of memory
        ui32 jump_address = upper_pc | target; // form full address
        pc = jump_address;
    }

    // sign extend utility function 
    ui32 sign_extend(const ui16& imm){
        ui32 sign_extended_imm = imm;
        if (imm & 0x8000) // if negative (16th bit 1)
            sign_extended_imm |= 0xFFFF0000; //trailiong 1's (-)
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
                std::copy(buffer.begin(), buffer.end(), mem[address]); // copy from buffer to memory
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

};

#endif
    
