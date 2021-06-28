#include "Instruction.h"

u8 ArmInstruction::cond()
{
    u8 cond = ((this->encoding) >> 28) & 0xF;
    return cond;
}

u8 ArmInstruction::i()
{
    u8 i = ((this->encoding) >> 25) & 0x1;
    return i;
}

u8 ArmInstruction::s()
{
    u8 s = ((this->encoding) >> 20) & 0x1;
    return s;
}

u8 ArmInstruction::rn()
{
    u8 rn = ((this->encoding) >> 16) & 0xF;
    return rn;
}

u8 ArmInstruction::rd()
{
    u8 rd = ((this->encoding) >> 12) & 0xF;
    return rd;
}

u16 ArmInstruction::operand2()
{
    u16 op2 = (this->encoding) & 0xFFF;
    return op2;
}

u8 ArmInstruction::shiftAmount()
{
    u8 shift = ((this->encoding) >> 7) & 0x1F;
    return shift;
}

u8 ArmInstruction::shiftType()
{
    u8 type = ((this->encoding) >> 5) & 0x3;
    return type;
}

u8 ArmInstruction::rm()
{
    u8 rm = (this->encoding) & 0xF;
    return rm;
}

u8 ArmInstruction::rotate()
{
    u8 rotate = ((this->encoding) >> 8) & 0xF;
    return rotate;
}

u8 ArmInstruction::imm()
{
    u8 imm = (this->encoding) & 0xFF;
    return imm;
}
