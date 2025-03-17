#ifndef FLAGS
#define FLAGS

#include <cstdint>

#define ui32 uint32_t

class Flags{
    private:
        bool zeroFlag;
        bool negativeFlag;
        bool overflowFlag;
        bool carryFlag;
        bool exceptionFlag;
        bool ltzFlag;
        bool gtzFlag;

    public:
        Flags(); // constructor
        
        // set all flags according to scenearios
        void setFlags(ui32,ui32,ui32);

        // set flags
        void setZeroFlag(bool);
        void setNegativeFlag(bool);
        void setOverflowFlag(bool);
        void setCarryFlag(bool);
        void setExceptionFlag(bool);
        void setLtzFlag(bool);
        void setGtzFlag(bool);

        // get flags
        bool getZeroFlag() const;
        bool getNegativeFlag() const;
        bool getOverflowFlag() const;
        bool getCarryFlag() const;
        bool getExeptionFlag() const;
        bool getLtzFlag() const;
        bool getGtzFlag() const;

        // reset flags
        void resetFlags();
};

#include "flags.cpp"
#endif