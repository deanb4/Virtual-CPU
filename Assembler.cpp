// Assembler Class
// Created by Carter on 3/18/2025.
// logic for assembler, turns assembly instructions into binary

#include "Assembler.h"
#include "opcodes.h"

// constructor
Assembler::Assembler(const string& asm_instructions, const string& divider) {
    instructions_input_debug = asm_instructions;
    separator_debug = divider;
    inst_copy = "";
    instructions = parser(asm_instructions, divider);
}


//ONLY USED FOR TESTING AS OF NOW CURRENTLY UNFINISHED
int Assembler::encodeInstructions(vector<uint32_t> & encoded_instructions) {//TODO instead of killing program, let it continue and put failures in vector of ints
    char instruction_type;
    string opcode;
    string encoded_instruction;

    for (int i = 0; i < instructions.size(); i++) {
        if (!getOpAndInstructionType(instructions[i], opcode, instruction_type)) {return i;}
        
        switch (instruction_type) {
            case 'R':
                encoded_instruction = rType(instructions[i], opcode);
                break;
            case 'I':
                encoded_instruction = iType(instructions[i], opcode);
                break;
            case 'J':
                encoded_instruction = jType(instructions[i], opcode);
                break;
            case 'T':
                encoded_instruction = trap(instructions[i], opcode);
                break;
            default:
                return i;
        }

        // la case
        if (mult_inst){
            uint32_t full_encode2;
            string ori;
            std::vector<string> new_inst = parser(encoded_instruction, "\n");
            string lui = new_inst[0];
            uint32_t full_encode1= stoll(lui,nullptr,2);
            encoded_instructions.push_back(full_encode1);
            if (new_inst.size() > 1){
                ori = new_inst[1]; 
                full_encode2= std::stoll(ori, nullptr, 2);
                encoded_instructions.push_back(full_encode2);
            }
            mult_inst = false;
        } else {

            if (encoded_instruction == "N") {return i;}

            uint32_t full_encode = stoll(encoded_instruction, nullptr, 2);

            encoded_instructions.push_back(full_encode);
        }

    }

    return -1;
}


/*********************
* Parses by placing each individual string in a vector of strings, based on the separator
* @param separator (string) str (string)
* @return void
**********************/
vector<string> Assembler::parser(const string& str, const string& separator) {
    string temp = str;
    vector<string> tokens;

    int i = 0;

    size_t pos = 0;

    while ((pos = temp.find(separator)) != string::npos) {
        string inst = temp.substr(0, pos);
        tokens.push_back(inst);
        temp.erase(0, pos + separator.length());
        i++;
    }

    if (!temp.empty()) {tokens.push_back(temp);}

    return tokens;
}


/*********************
* Looks for first occurrence of character, then truncates rest of string to the right and returns to caller,
* DOES NOT EDIT ORIGINAL STRING
* @param separator (char) str (string)
* @return string
**********************/
std::string Assembler::parser(const std::string& str, const char separator) {
    size_t pos = str.find(separator);
    return (pos != string::npos) ? str.substr(0, pos) : str;
}


/*********************
* function takes an assembly instruction represented as a string and returns the opcode and instruction type
* returns t/f if nothing is found in map
* @param instruction (string) opcode (string) instructions_type (char)
* @return bool
**********************/
bool Assembler::getOpAndInstructionType(const string& instruction, string& opcode, char& instruction_type) {
    bool flag = true;

    const map<string, pair<string, char>> opcode_encode {
        // R type
        {"add", {"100000", 'R'}},
        {"sub", {"100010", 'R'}},
        {"mult", {"011000", 'R'}},
        {"div", {"011010", 'R'}},
        {"mfhi", {"010000", 'R'}},
        {"mflo", {"010010", 'R'}},
        {"and", {"100100", 'R'}},
        {"or", {"100101", 'R'}},
        {"xor", {"100110", 'R'}},
        {"nor", {"100111", 'R'}},
        {"sll", {"000000", 'R'}},
        {"srl", {"000010", 'R'}},
        {"jalr", {"001001", 'R'}},
        {"jr", {"001011", 'R'}},

        // I type
        {"addi", {"001000", 'I'}},
        {"andi", {"001100", 'I'}},
        {"ori", {"001101", 'I'}},
        {"xori", {"001110", 'I'}},
        {"lw", {"100011", 'I'}},
        {"sw", {"101011", 'I'}},
        {"lb", {"010001", 'I'}},
        {"lh", {"100001", 'I'}},
        {"sb", {"101000", 'I'}},
        {"sh", {"101001", 'I'}},
        {"beq", {"000100", 'I'}},
        {"bne", {"000101", 'I'}},
        {"bgez", {"000001", 'I'}},
        {"bgtz", {"000111", 'I'}},
        {"blez", {"000110", 'I'}},
        {"bltz", {"111100", 'I'}},
        {"lui", {"001111", 'I'}},
        {"li", {"111001", 'I'}}, // skip for now
        {"la", {"000000", 'I'}}, 

        // J type
        {"j", {"110011", 'J'}},
        {"jal", {"000011", 'J'}},

        // Trap (syscall)
        {"syscall", {"111111", 'T'}},
    };

    if (instruction == "syscall")
    {int f = 3;}

    const string inst = parser(instruction, ' ');

    try {
        opcode = opcode_encode.at(inst).first;
        instruction_type = opcode_encode.at(inst).second;

    } catch (...) {
        flag = false;
        opcode = "-1";
        instruction_type = 'N';
    }

    return flag;
}


/*********************
* returns t/f based on if a user tries to store a result in an invalid register
* @param rd (constr string)
* @return bool
**********************/
bool Assembler::validStoreReg(const string& rd) {
    vector<string> invalid_registers {"$zero", "$at", "$fp", "$gp", "$ra", "$k0", "$k1"};
    bool flag = true;

    for (int i = 0; i < invalid_registers.size() && flag; ++i) {flag = invalid_registers[i] != rd;}

    return flag;
};


/*********************
* takes any base number (that can be declared natively, bin, dec, oct, hex) as a string, converts it to a binary string,
* and returns t/f if successful,and the number fits the instruction
* @param any_base (const string), bits (const int), binary_string (string)
* @return bool
**********************/
bool Assembler::anyBaseToBinary(const std::string& any_base, const int bits, std::string& binary_string) {
    bool flag = true;
    string temp = any_base;
    int base = 10;
    int negative_check = 0;
    char twos_complement = '1';

    if (any_base[0] == '-') {negative_check = 1; twos_complement = '0';}

    if (any_base.substr(negative_check, 2) == "0x") {
        base = 16;
        temp.erase(negative_check, 2);

    } else if (any_base.substr(negative_check, 2) == "0b") {
        base = 2;
        temp.erase(negative_check, 2);

    } else if (any_base[negative_check] == '0' && any_base.length() > 1) {
        base = 8;
        temp.erase(negative_check, 1);
    }

    try {
        size_t pos = 0;
        const int imm_value = std::stoi(temp, &pos, base);

        if (pos != temp.length()) {throw std::invalid_argument("Invalid characters in input string");}

        const std::bitset<26> binary_int(imm_value);
        binary_string = binary_int.to_string();

        size_t imm_size = 26 - (binary_string.find(twos_complement) + negative_check);

        if (imm_size <= static_cast<size_t>(bits) || any_base == "0") {binary_string.erase(0, 26 - bits);}

        else {throw std::invalid_argument("Immediate value too large for specified bit width");}

    } catch (...) {flag = false;}

    return flag;
}


/*********************
* takes a vector of strings and strips the whitespace out of each element
* @param vec_str (vector of strings)
* @return string
**********************/
void Assembler::stripWhiteSpaces(vector<string>& vec_str) {//TODO potentially combine with parser and add a t/f param
    for (auto & i : vec_str) {i.erase(std::remove(i.begin(),i.end(),' '));}
}


/*********************
* takes rtype instruction and encodes the rd, rs, rt, and shamt, returns N given invalid instruction
* @param reg (const string), opcode (constr string)
* @return string
**********************/
string Assembler::rType(const string &reg, const string& opcode) const {
    //flag to make sure specific unique rtype instructions are correct,
    string syntax_flag = parser(reg, ' ');

    //remove instruction so only registers and consts are left, add comma to end of
    // TODO see about moving this encodeInstructions method
    string temp = reg;
    const size_t i = temp.find(' ');
    temp.erase(0,i);

    //TODO see about moving some of this into a struct
    vector<string> registers = parser(temp, ",");

    stripWhiteSpaces(registers);

    string encoded_registers;

    const string rtype = "000000";
    string rs = "00000";
    string rt = "00000";
    string rd = "00000";
    string shamt = "00000";

    try {
        const size_t argc = registers.size();

        constexpr int MAX_BITSIZE = 5; 

        if (argc == 0 || argc > 3) {throw std::invalid_argument("Invalid number of args");}

        if (
            ((syntax_flag != "jalr" || syntax_flag != "div" || syntax_flag != "mult" || syntax_flag != "jr") && !validStoreReg(registers[0])) ||
             (syntax_flag == "jalr" && !validStoreReg(registers[1]))
            )
            {throw std::invalid_argument("Invalid rd register");}

        if (
            ((syntax_flag == "div"  || syntax_flag == "mult" || syntax_flag == "jalr") && argc != 2) ||
            ((syntax_flag == "mflo" || syntax_flag == "mfhi" || syntax_flag == "jr") && argc != 1)
            )
            {throw std::invalid_argument("Invalid number of args");}

        if (
            (syntax_flag == "sll" || syntax_flag == "srl") &&
            (argc != 3 || !anyBaseToBinary(registers[2],MAX_BITSIZE, shamt))
            )
            {throw std::invalid_argument("Invalid imm or number of args");}

        if (syntax_flag == "mult" || syntax_flag == "div") {
            rs = register_encode_map.at(registers[0]);
            rt = register_encode_map.at(registers[1]);

        } else if (syntax_flag == "jalr") {
            rs = register_encode_map.at(registers[0]);
            rd = register_encode_map.at(registers[1]);

        } else if (syntax_flag == "sll" || syntax_flag == "srl") {
            rd = register_encode_map.at(registers[0]);
            rt = register_encode_map.at(registers[1]);

        } else if (syntax_flag == "mflo" || syntax_flag == "mfhi") {
            rd = register_encode_map.at(registers[0]);

        } else if (syntax_flag == "jr") {
            rs = register_encode_map.at(registers[0]);

        } else if (argc == 3) {
            rd = register_encode_map.at(registers[0]);
            rs = register_encode_map.at(registers[1]);
            rt = register_encode_map.at(registers[2]);

        } else {throw std::invalid_argument("Unknown syntax");}

        encoded_registers = rtype + rs + rt + rd + shamt + opcode;

    } catch (...) {encoded_registers = "N";}

    return encoded_registers;
}


/*********************
* takes itype instruction and encodes the rs, rt, imm, returns N if given invalid instruction
* @param reg (const string), opcode (constr string)
* @return string
**********************/
string Assembler::iType(const string &reg, const string& opcode) const {
    //flag to make sure specific unique itype instructions are correct,
    string syntax_flag = parser(reg, ' ');
    bool check = false;
    string temp_op= "";
    string instruction_two;
    //remove instruction so only registers and consts are left
    // TODO see about moving this encodeInstructions method
    string temp = reg;
    const size_t i = temp.find(' ');
    temp.erase(0,i);

    //TODO see about moving some of this into a struct
    vector<string> registers = parser(temp, ",");

    stripWhiteSpaces(registers);

    string encoded_registers;

    string rt = "00000";
    string rs = "00000";
    string imm = "0000000000000000";

    try {
        const size_t argc = registers.size();

        const int opcode_int = std::stoi(opcode, nullptr, 2);

        constexpr int MAX_BITSIZE = 16;

        if (argc == 3) {
            switch (opcode_int) {
                case ADDI:
                    if (!validStoreReg(registers[0])) {throw std::invalid_argument("Invalid register");}
                    if (!anyBaseToBinary(registers[2], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rt = register_encode_map.at(registers[0]);
                    rs = register_encode_map.at(registers[1]);
                break;

                case ANDI:
                    if (!validStoreReg(registers[0])) {throw std::invalid_argument("Invalid register");}
                    if (!anyBaseToBinary(registers[2], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rt = register_encode_map.at(registers[0]);
                    rs = register_encode_map.at(registers[1]);
                break;

                case ORI:
                    if (!validStoreReg(registers[0])) {throw std::invalid_argument("Invalid register");}
                    if (!anyBaseToBinary(registers[2], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rt = register_encode_map.at(registers[0]);
                    rs = register_encode_map.at(registers[1]);    
                break;

                case XORI:
                    if (!validStoreReg(registers[0])) {throw std::invalid_argument("Invalid register");}
                    if (!anyBaseToBinary(registers[2], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rt = register_encode_map.at(registers[0]);
                    rs = register_encode_map.at(registers[1]);
                break;

                case BEQ:
                    if (!anyBaseToBinary(registers[2], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rs = register_encode_map.at(registers[0]);
                    rt = register_encode_map.at(registers[1]);
                break;

                case BNE:
                    if (!anyBaseToBinary(registers[2], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rs = register_encode_map.at(registers[0]);
                    rt = register_encode_map.at(registers[1]);
                break;

                default:
                    throw std::invalid_argument("Invalid opcode");
                break;
            }

        } else if (argc == 2) {
            //TODO, to get rid of syntax flag, refactor this block into its own method and call it in the switch case where needed
            if (registers.back().back() == ')' && syntax_flag[0] != 'b') {
                vector<string> imm_extract = parser(registers[1], "(");
                imm_extract.back().pop_back();

                registers.pop_back();
                registers.insert(registers.end(), imm_extract.begin(), imm_extract.end());
            }

            switch (opcode_int) {
                case LW:
                    if (!validStoreReg(registers[0])) {throw std::invalid_argument("Invalid register");}
                    if (!anyBaseToBinary(registers[1], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rt = register_encode_map.at(registers[0]);
                    rs = register_encode_map.at(registers[2]);
                break;

                case SW:
                    if (!anyBaseToBinary(registers[1], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rt = register_encode_map.at(registers[0]);
                    rs = register_encode_map.at(registers[2]);
                break;

                case LB:
                    if (!validStoreReg(registers[0])) {throw std::invalid_argument("Invalid register");}
                    if (!anyBaseToBinary(registers[1], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rt = register_encode_map.at(registers[0]);
                    rs = register_encode_map.at(registers[2]);
                break;

                case LH:
                    if (!validStoreReg(registers[0])) {throw std::invalid_argument("Invalid register");}
                    if (!anyBaseToBinary(registers[1], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rt = register_encode_map.at(registers[0]);
                    rs = register_encode_map.at(registers[2]);
                break;

                case SB:
                    if (!anyBaseToBinary(registers[1], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rt = register_encode_map.at(registers[0]);
                    rs = register_encode_map.at(registers[2]);
                break;

                case SH:
                    if (!anyBaseToBinary(registers[1], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rt = register_encode_map.at(registers[0]);
                    rs = register_encode_map.at(registers[2]);
                break;

                case BGEZ:
                    if (!anyBaseToBinary(registers[1], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rs = register_encode_map.at(registers[0]);
                    rt = "00001";
                break;

                case BGTZ:
                    if (!anyBaseToBinary(registers[1], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rs = register_encode_map.at(registers[0]);
                    rt = "00000";
                break;

                case BLEZ:
                    if (!anyBaseToBinary(registers[1], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rs = register_encode_map.at(registers[0]);
                    rt = "00000";
                break;

                case BLTZ:
                    if (!anyBaseToBinary(registers[1], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rs = register_encode_map.at(registers[0]);
                    rt = "00000";
                break;

                case LUI:
                    if (!anyBaseToBinary(registers[1], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rt = register_encode_map.at(registers[0]);
                    rs = "00000";
                break;

                case LI:
                    if (!anyBaseToBinary(registers[1], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rt = register_encode_map.at(registers[0]);
                    rs = "00000";
                break;
                
                // la case: turn instruction into lui case (need to get it to add ori too if needed)
                case 000000:
                    std::cout << "case 000000" << std::endl;
                    temp_op = "001111";
                    check = true;
                    if (!anyBaseToBinary(registers[1], MAX_BITSIZE, imm)) {throw std::invalid_argument("Invalid imm");}
                    rt = register_encode_map.at(registers[0]);
                    rs = "00000";
                    // TODO fix so that instruction two returns to main function and is pushed into instruction vector after lui
                    if (std::stoll(imm) > (1 << 16)){
                        instruction_two = "001101" + rt + rt + imm;
                        std::cout << std::hex << instruction_two << std::endl;
                    }
                break;

                default:
                    throw std::invalid_argument("Invalid opcode");
                break;
            }

        } else {throw std::invalid_argument("Unknown syntax");}

        if (check){
            encoded_registers = temp_op + rs + rt + imm + "\n" + instruction_two;
            mult_inst = true;
            
        } else
            encoded_registers = opcode + rs + rt + imm;

    } catch (...) {encoded_registers = "N";}

    // inst_copy = instruction_two;
    return encoded_registers;
}


/*********************
* takes jtype instruction and encodes the target, returns N  if given invalid instruction
* @param reg (const string), opcode (constr string)
* @return string
**********************/
string Assembler::jType(const string& reg, const string& opcode) const {
    // TODO see about moving this encodeInstructions method
    string temp = reg;
    const size_t i = temp.find(' ');
    temp.erase(0,i);

    //TODO see about moving some of this into a struct
    vector<string> registers = parser(temp, ",");

    stripWhiteSpaces(registers);

    string encoded_registers;

    string target = "00000000000000000000000000";

    try {
        const size_t argc = registers.size();

        const int opcode_int = std::stoi(opcode, nullptr, 2);

        constexpr int MAX_BITSIZE = 26;

        // leaving this logic in incase we add more jTypes later
        if (argc == 1) {
            switch (opcode_int) {
                case J:
                    if (!anyBaseToBinary(registers[0], MAX_BITSIZE, target)) {throw std::invalid_argument("Invalid target");}
                break;

                case JAL:
                    if (!anyBaseToBinary(registers[0], MAX_BITSIZE, target)) {throw std::invalid_argument("Invalid target");}
                break;

                default:
                    throw std::invalid_argument("Invalid opcode");
                break;
            }

        } else {throw std::invalid_argument("Unknown syntax");}

        encoded_registers = opcode + target;

    } catch (...) {encoded_registers = "N";}

    return encoded_registers;
}


/*********************
* returns only implemented trap instruction. Will refactor if more are added
* @param reg (const string), opcode (constr string)
* @return string
**********************/
string Assembler::trap(string& reg, const string& opcode) {return "11111100000000000000000000000000";}