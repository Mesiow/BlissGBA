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

u8 ArmInstruction::opcode()
{
    u8 op = ((this->encoding) >> 21) & 0xF;
    return u8();
}

u8 ArmInstruction::s()
{
    u8 s = ((this->encoding) >> 20) & 0x1;
    return s;
}

RegisterID ArmInstruction::rn()
{
    RegisterID rn;
    rn.id = ((this->encoding) >> 16) & 0xF;
    return rn;
}

RegisterID ArmInstruction::rd()
{
    RegisterID rd;
    rd.id = ((this->encoding) >> 12) & 0xF;
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

RegisterID ArmInstruction::rs()
{
    RegisterID rs;
    rs.id = (this->encoding >> 8) & 0xF;
    return rs;
}

RegisterID ArmInstruction::rm()
{
    RegisterID rm;
    rm.id = (this->encoding) & 0xF;
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

u8 ArmInstruction::branch()
{
    u8 ins = ((this->encoding) >> 25) & 0x7;
    return ins;
}

u8 ArmInstruction::l()
{
    u8 l = ((this->encoding) >> 24) & 0x1;
    return l;
}

s32 ArmInstruction::offset()
{
    s32 offset = (this->encoding) & 0xFFFFFF;
    return offset;
}

u8 ArmInstruction::bits4To7()
{
    u8 bits4to7 = ((this->encoding) >> 4) & 0xF;
    return bits4to7;
}

u8 ArmInstruction::bits20To27()
{
    u8 bits20to27 = ((this->encoding) >> 20) & 0xFF;
    return bits20to27;
}
