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
    return op;
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

u16 ArmInstruction::instruction()
{
    u8 b20to27 = ((this->encoding) >> 20) & 0xFF;
    u8 b4to7 = ((this->encoding) >> 4) & 0xF;
    u16 combined = ((b20to27 << 4) | b4to7) & 0xFFF;
    
    return combined;
}

u8 ArmInstruction::bits8to11()
{
    u8 b8to11 = ((this->encoding) >> 8) & 0xF;
    return b8to11;
}

u8 ArmInstruction::P()
{
    u8 P = ((this->encoding) >> 24) & 0x1;
    return P;
}

u8 ArmInstruction::U()
{
    u8 U = ((this->encoding) >> 23) & 0x1;
    return U;
}

u8 ArmInstruction::W()
{
    u8 W = ((this->encoding) >> 21) & 0x1;
    return W;
}

u8 ArmInstruction::L()
{
    u8 L = ((this->encoding) >> 20) & 0x1;
    return L;
}

u8 ArmInstruction::S()
{
    u8 S = ((this->encoding) >> 6) & 0x1;
    return S;
}

u8 ArmInstruction::H()
{
    u8 H = ((this->encoding) >> 5) & 0x1;
    return H;
}
