#include "Arm.h"
#include "AddressingModes.h"


AddressingMode::AddressingMode(Arm& cpu)
	:cpu(cpu)
{

}

AddressingMode1::AddressingMode1(Arm& cpu)
	:AddressingMode(cpu)
{
	
}


u32 AddressingMode1::shift(ArmInstruction& ins, u8& shiftedBit)
{
	bool regShift = isRegisterShift(ins);

	if (regShift) {
		//Register shift
		u8 shiftType = ins.shiftType();
		u8 rm = ins.rm();
		u8 rs = ins.rs();

		u32 rm_reg = cpu.getRegister(rm);
		u32 rs_reg = cpu.getRegister(rs);

		//The least significant byte of rs is the shift amount
		rm_reg = cpu.shift(rs_reg, ((rs_reg) & 0xFF), shiftType, shiftedBit);
	}
	else {
		//Immediate shift
		u8 shiftAmount = ins.shiftAmount();
		u8 shiftType = ins.shiftType();
		u8 rm = ins.rm();

		u32 rm_reg = cpu.getRegister(rm);
		rm_reg = cpu.shift(rm_reg, shiftAmount, shiftType, shiftedBit);

		return rm_reg;
	}
}

u8 AddressingMode1::imm(ArmInstruction& ins, u8& shiftedBit)
{
	u8 rotate = ins.rotate();
	u8 imm = ins.imm();

	if (rotate == 0)
		return imm;

	imm = cpu.ror(imm, rotate);
	shiftedBit = (imm >> 31) & 0x1;

	return imm;
}

u8 AddressingMode1::isRegisterShift(ArmInstruction& ins)
{
	u16 op2 = ins.operand2();
	u8 bit4 = ((op2 >> 4) & 0x1);
	return bit4;
}

