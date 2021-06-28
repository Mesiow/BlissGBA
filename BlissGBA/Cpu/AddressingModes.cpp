#include "AddressingModes.h"
#include "Arm.h"

AddressingMode1::AddressingMode1(Arm& cpu)
	:cpu(cpu)
{
	
}


u32 AddressingMode1::regShift(ArmInstruction& ins, u8& shiftedBit)
{
	u8 shiftAmount = ins.shiftAmount();
	u8 shiftType = ins.shiftType();
	u8 rm = ins.rm();

	u32 rm_reg = cpu.getRegister(rm);
	rm_reg = cpu.shift(rm_reg, shiftAmount, shiftType, shiftedBit);

	return rm_reg;
}

u8 AddressingMode1::imm(ArmInstruction& ins, u8& shiftedBit)
{
	u8 rotate = ins.rotate();
	u8 imm = ins.imm();

	shiftedBit = (rotate - 1);
	shiftedBit = ((imm >> shiftedBit) & 0x1);
	imm >>= rotate;

	return imm;
}
