// Assembler Class
// Created by Carter on 3/18/2025.
// header file for assembler class

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <bitset>
#include <ranges>
#include "opcodes.h"
#include <ranges>

using std::string;
using std::vector;
using std::map;
using std::pair;

//TODO ADD STRUCT FOR VARS

class Assembler {

public:
    //default constructor
    Assembler() = default;

    explicit Assembler(const string&, const string& = "\n");

    int encodeInstructions(vector<uint32_t>&);

private:
    static vector<string> parser(const string&, const string&);

    static string parser(const string&, char);

    static bool getOpAndInstructionType(const string&, string&, char&);

    static bool validStoreReg(const string&);

    static bool anyBaseToBinary(const string&, int, string&);

    static void stripWhiteSpaces(vector<string>&);

    [[nodiscard]] string rType(const string&, const string&) const;

    [[nodiscard]] string iType(const string&, const string&) const;

    string jType(const string&, const string&) const;

    string trap(string&, const string&);

    // attributes

    vector<string> instructions;

    const map<string, string> register_encode_map {
            {"$zero", "00000"},
            {"$at",   "00001"},
            {"$v0",   "00010"},
            {"$v1",   "00011"},
            {"$a0",   "00100"},
            {"$a1",   "00101"},
            {"$a2",   "00110"},
            {"$a3",   "00111"},
            {"$t0",   "01000"},
            {"$t1",   "01001"},
            {"$t2",   "01010"},
            {"$t3",   "01011"},
            {"$t4",   "01100"},
            {"$t5",   "01101"},
            {"$t6",   "01110"},
            {"$t7",   "01111"},
            {"$s0",   "10000"},
            {"$s1",   "10001"},
            {"$s2",   "10010"},
            {"$s3",   "10011"},
            {"$s4",   "10100"},
            {"$s5",   "10101"},
            {"$s6",   "10110"},
            {"$s7",   "10111"},
            {"$t8",   "11000"},
            {"$t9",   "11001"},
            {"$k0",   "11010"},
            {"$k1",   "11011"},
            {"$gp",   "11100"},
            {"$sp",   "11101"},
            {"$fp",   "11110"},
            {"$ra",   "11111"}
    };

    //used for debug purposes
    string separator_debug;
    string instructions_input_debug;
};

#include "Assembler.cpp"
#endif //ASSEMBLER_H
