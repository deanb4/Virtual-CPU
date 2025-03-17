#include "flags.h"

Flags::Flags(): zeroFlag(false), negativeFlag(false), overflowFlag(false), carryFlag(false),
                exceptionFlag(false), ltzFlag(false),gtzFlag(false) 
                {
                    // constructor -> Initializes all flags to false

                } 


// add all flags***
void Flags::setFlags(ui32 r1, ui32 r2, ui32 r3){
    // Check for Zero flag
    if (r1 == 0) {
        setZeroFlag(true);
    }

    // Check for Overflow flag using the utility function
    if (utils::checkIntOverflow(r1, r2, r3)) {
        setOverflowFlag(true);
    }

    // Check for Negative flag (using a helper function or inline bitwise operation)
    if (r1 & 0x80000000) {
        setNegativeFlag(true);
    }

    // Carry flag: checks for unsigned integer overflow
    if (r1 < r2){
        setCarryFlag(true);
    }

    // ltzFlag: check if result is less than zero
    if (r1 < 0){
        setLtzFlag(true);
    } 

    // gtzFlag: check if result is greater than zero
    if (r1> 0){
        setGtzFlag(true);
    }
    
}

// set flags
void Flags::setZeroFlag(bool value){
    zeroFlag = value;
}
void Flags::setNegativeFlag(bool value){
    negativeFlag = value;
}
void Flags::setOverflowFlag(bool value){
    overflowFlag = value;
}
void Flags::setCarryFlag(bool value){
    carryFlag = value;
}
void Flags::setExceptionFlag(bool value){
    exceptionFlag = value;
}
void Flags::setLtzFlag(bool value){
    ltzFlag = value;
}
void Flags::setGtzFlag(bool value){
    gtzFlag = value;
}


// get flags
bool Flags::getZeroFlag() const {
    return zeroFlag;
}

bool Flags::getNegativeFlag() const{
    return negativeFlag;
}

bool Flags::getOverflowFlag() const{
    return overflowFlag;
}

bool Flags::getCarryFlag() const{
    return carryFlag;
}

bool Flags::getExeptionFlag() const{
    return exceptionFlag;
}

bool Flags::getLtzFlag() const{
    return ltzFlag;
}

bool Flags::getGtzFlag() const{
    return gtzFlag;
}

// reset flags
void Flags::resetFlags() {
    zeroFlag = false;
    negativeFlag = false;
    overflowFlag = false;
    carryFlag = false;
    exceptionFlag = false;
    ltzFlag = false;
    gtzFlag = false;
}