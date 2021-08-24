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
		RegisterID rm = ins.rm();
		RegisterID rs = ins.rs();

		u32 rm_reg = cpu.getRegister(rm);
		u32 rs_reg = cpu.getRegister(rs);

		//The least significant byte of rs is the shift amount
		rm_reg = cpu.shift(rs_reg, ((rs_reg) & 0xFF), shiftType, shiftedBit);

		return rm_reg;
	}
	else {
		//Immediate shift
		u8 shiftAmount = ins.shiftAmount();
		u8 shiftType = ins.shiftType();
		RegisterID rm = ins.rm();
		RegisterID rn = ins.rn();

		u32 rm_reg = cpu.getRegister(rm);
		u32 rn_reg = cpu.getRegister(rn);

		if (shiftAmount == 0) {
			shiftedBit = cpu.getFlag(C);
			return rm_reg;
			/*u32 r15 = cpu.getRegister(RegisterID{ R15_ID });
			if ((rm_reg == r15) || (rn_reg == r15)) {
				rm_reg = r15;
			}
			else
				rm_reg = cpu.rrx(rm_reg, shiftedBit);*/
		}
		else {
			rm_reg = cpu.shift(rm_reg, shiftAmount, shiftType, shiftedBit);
		}
		return rm_reg;
	}
}

u32 AddressingMode1::imm(ArmInstruction& ins, u8& shiftedBit)
{
	u8 rotate = ins.rotate();
	u32 imm = ins.imm();

	if (rotate == 0) {
		shiftedBit = cpu.getFlag(C);
		return imm;
	}

	imm = cpu.ror(imm, rotate * 2);
	shiftedBit = (imm >> 31) & 0x1;

	return imm;
}

u8 AddressingMode1::isRegisterShift(ArmInstruction& ins)
{
	u16 op2 = ins.operand2();
	u8 bit4 = ((op2 >> 4) & 0x1);
	return bit4;
}

AddressingMode3::AddressingMode3(Arm& cpu)
	:AddressingMode(cpu)
{

}

AddrMode3Result AddressingMode3::immOffsetIndex(ArmInstruction& ins)
{
	RegisterID rn = ins.rn();
	u32 rn_reg = cpu.getRegister(rn);

	u8 P = ins.P();
	u8 W = ins.W();
	u8 U = ins.U();
	u8 immH = ins.immedH();
	u8 immL = ins.immedL();

	AddrMode3Result result;
	u32 address;
	u8 offset_8 = (immH << 4) | immL;
	if (P == 0x1) {
		//Pre-indexed addressing (mem address is written back to the base register)
		if (W == 0x1) {
			address = ((U == 0x1) ? rn_reg + offset_8 : rn_reg - offset_8);
			result.type = AddrMode3Type::PREINDEXED;
			result.address = address;
		}
		//Offset addressing (the base register is unchanged)
		else {
			address = ((U == 0x1) ? rn_reg + offset_8 : rn_reg - offset_8);
			result.type = AddrMode3Type::OFFSET;
			result.address = address;
		}
	}
	else {
		if (W == 0x0) {
			//Post-indexed addressing (The base reg value is used for the mem address,
			//and the offset is applied to the base reg value and written back to the base reg)
			address = rn_reg;
			result.rn = ((U == 0x1) ? rn_reg + offset_8 : rn_reg - offset_8);
			result.type = AddrMode3Type::POSTINDEX;
			result.address = address;
		}
		//If W == 1 (unpredictable behavior)
	}

	return result;
}

AddrMode3Result AddressingMode3::registerOffsetIndex(ArmInstruction& ins)
{
	RegisterID rn = ins.rn();
	RegisterID rm = ins.rm();

	u32 rn_reg = cpu.getRegister(rn);
	u32 rm_reg = cpu.getRegister(rm);

	u8 P = ins.P();
	u8 W = ins.W();
	u8 U = ins.U();

	AddrMode3Result result;
	u32 address;
	if (P == 0x1) {
		//Pre-indexed addressing (mem address is written back to the base register)
		if (W == 0x1) {
			address = ((U == 0x1) ? rn_reg + rm_reg : rn_reg - rm_reg);
			result.type = AddrMode3Type::PREINDEXED;
			result.address = address;
		}
		//Offset addressing (the base register is unchanged)
		else {
			address = ((U == 0x1) ? rn_reg + rm_reg : rn_reg - rm_reg);
			result.type = AddrMode3Type::OFFSET;
			result.address = address;
		}
	}
	else {
		//Post-indexed addressing (The base reg value is used for the mem address,
		//and the offset is applied to the base reg value and written back to the base reg)
		if (W == 0x0) {
			address = rn_reg;
			result.rn = ((U == 0x1) ? rn_reg + rm_reg : rn_reg - rm_reg);
			result.type = AddrMode3Type::POSTINDEX;
			result.address = address;
		}
		//If W == 1 (unpredictable behavior)
	}

	return result;
}
