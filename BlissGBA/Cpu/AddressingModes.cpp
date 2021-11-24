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
		rm_reg = cpu.shift(rm_reg, ((rs_reg) & 0xFF), shiftType, shiftedBit);

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
			
			u32 r15 = cpu.R15;
			if ((rm_reg == r15) || (rn_reg == r15)) {
				rm_reg = r15;
			}
			return rm_reg;
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

AddressingMode2::AddressingMode2(Arm& cpu)
	:AddressingMode(cpu)
{

}

AddrModeLoadStoreResult AddressingMode2::immOffsetIndex(ArmInstruction& ins)
{
	RegisterID rn = ins.rn();
	u32 rn_reg = cpu.getRegister(rn);

	u8 P = ins.P();
	u8 W = ins.W();
	u8 U = ins.U();

	u16 offset_12 = ins.offset12();
	AddrModeLoadStoreResult result = {AddrModeLoadStoreType::NONE, 0, 0, 0};
	u32 address;
	if (P == 0x1) {
		//Pre-indexed addressing (mem address is written back to the base register)
		if (W == 0x1) {
			address = ((U == 0x1) ? rn_reg + offset_12 : rn_reg - offset_12);
			result.type = AddrModeLoadStoreType::PREINDEXED;
			result.address = address;
		}
		//Offset addressing (the base register is unchanged)
		else {
			address = ((U == 0x1) ? rn_reg + offset_12 : rn_reg - offset_12);
			result.type = AddrModeLoadStoreType::OFFSET;
			result.address = address;
		}
	}
	else {
		//Post-indexed addressing (The base reg value is used for the mem address,
		//and the offset is applied to the base reg value and written back to the base reg)
		if (W == 0x0) {
			//LDR, LDRB, STR or STRB (normal memory access performed)
			address = rn_reg;
			result.rn = ((U == 0x1) ? rn_reg + offset_12 : rn_reg - offset_12);
			result.type = AddrModeLoadStoreType::POSTINDEX;
			result.address = address;
		}
		else {
			//LDRBT, LDRT, STRBT or STRT (unprivileged (User mode) memory access performed))
		}
	}

	return result;
}


AddrModeLoadStoreResult AddressingMode2::scaledRegisterOffsetIndex(ArmInstruction& ins)
{
	RegisterID rm = ins.rm();
	RegisterID rn = ins.rn();
	u32 rm_reg = cpu.getRegister(rm);
	u32 rn_reg = cpu.getRegister(rn);

	u8 P = ins.P();
	u8 W = ins.W();
	u8 U = ins.U();


	//if LSL 0, then regular register offset/index addressing mode
	//else, then it's scaled register offset/index

	bool scaledRegOffset = true;
	if (((ins.encoding >> 4) & 0xFF) == 0x0) { //Register offset
		scaledRegOffset = false;
	}

	AddrModeLoadStoreResult result = { AddrModeLoadStoreType::NONE, 0, 0, 0 };
	u32 address;
	if (P == 0x1) {
		if (scaledRegOffset) {
			u32 index = cpu.shift(rm_reg, ins.shiftAmount(), ins.shiftType(), result.shifterCarryOut);
			address = ((U == 0x1) ? rn_reg + index : rn_reg - index);
		}
		else {
			address = ((U == 0x1) ? rn_reg + rm_reg : rn_reg - rm_reg);
		}

		//Pre-indexed addressing (mem address is written back to the base register)
		if (W == 0x1) {
			result.type = AddrModeLoadStoreType::PREINDEXED;
			result.address = address;
		}
		//Offset addressing (the base register is unchanged)
		else {
			result.type = AddrModeLoadStoreType::OFFSET;
			result.address = address;
		}
	}
	else {
		//Post-indexed addressing (The base reg value is used for the mem address,
		//and the offset is applied to the base reg value and written back to the base reg)
		if (W == 0x0) {
			//LDR, LDRB, STR or STRB (normal memory access performed)
			address = rn_reg;
			if (scaledRegOffset) {
				u32 index = cpu.shift(rm_reg, ins.shiftAmount(), ins.shiftType(), result.shifterCarryOut);
				result.rn = ((U == 0x1) ? rn_reg + index : rn_reg - index);
			}
			else {
				result.rn = ((U == 0x1) ? rn_reg + rm_reg : rn_reg - rm_reg);
			}
			result.type = AddrModeLoadStoreType::POSTINDEX;
			result.address = address;
		}
		else {
			//LDRBT, LDRT, STRBT or STRT (unprivileged (User mode) memory access performed))
		}
	}

	return result;
}


AddressingMode3::AddressingMode3(Arm& cpu)
	:AddressingMode(cpu)
{

}

AddrModeLoadStoreResult AddressingMode3::immOffsetIndex(ArmInstruction& ins)
{
	RegisterID rn = ins.rn();
	u32 rn_reg = cpu.getRegister(rn);

	u8 P = ins.P();
	u8 W = ins.W();
	u8 U = ins.U();
	u8 immH = ins.immedH();
	u8 immL = ins.immedL();

	AddrModeLoadStoreResult result = { AddrModeLoadStoreType::NONE, 0, 0, 0 };
	u32 address;
	u8 offset_8 = (immH << 4) | immL;
	if (P == 0x1) {
		//Pre-indexed addressing (mem address is written back to the base register)
		if (W == 0x1) {
			address = ((U == 0x1) ? rn_reg + offset_8 : rn_reg - offset_8);
			result.type = AddrModeLoadStoreType::PREINDEXED;
			result.address = address;
		}
		//Offset addressing (the base register is unchanged)
		else {
			address = ((U == 0x1) ? rn_reg + offset_8 : rn_reg - offset_8);
			result.type = AddrModeLoadStoreType::OFFSET;
			result.address = address;
		}
	}
	else {
		if (W == 0x0) {
			//Post-indexed addressing (The base reg value is used for the mem address,
			//and the offset is applied to the base reg value and written back to the base reg)
			address = rn_reg;
			result.rn = ((U == 0x1) ? rn_reg + offset_8 : rn_reg - offset_8);
			result.type = AddrModeLoadStoreType::POSTINDEX;
			result.address = address;
		}
		//If W == 1 (unpredictable behavior)
	}

	return result;
}

AddrModeLoadStoreResult AddressingMode3::registerOffsetIndex(ArmInstruction& ins)
{
	RegisterID rn = ins.rn();
	RegisterID rm = ins.rm();

	u32 rn_reg = cpu.getRegister(rn);
	u32 rm_reg = cpu.getRegister(rm);

	u8 P = ins.P();
	u8 W = ins.W();
	u8 U = ins.U();

	AddrModeLoadStoreResult result = { AddrModeLoadStoreType::NONE, 0, 0, 0 };
	u32 address;
	if (P == 0x1) {
		//Pre-indexed addressing (mem address is written back to the base register)
		if (W == 0x1) {
			address = ((U == 0x1) ? rn_reg + rm_reg : rn_reg - rm_reg);
			result.type = AddrModeLoadStoreType::PREINDEXED;
			result.address = address;
		}
		//Offset addressing (the base register is unchanged)
		else {
			address = ((U == 0x1) ? rn_reg + rm_reg : rn_reg - rm_reg);
			result.type = AddrModeLoadStoreType::OFFSET;
			result.address = address;
		}
	}
	else {
		//Post-indexed addressing (The base reg value is used for the mem address,
		//and the offset is applied to the base reg value and written back to the base reg)
		if (W == 0x0) {
			address = rn_reg;
			result.rn = ((U == 0x1) ? rn_reg + rm_reg : rn_reg - rm_reg);
			result.type = AddrModeLoadStoreType::POSTINDEX;
			result.address = address;
		}
		//If W == 1 (unpredictable behavior)
	}

	return result;
}

AddressingMode4::AddressingMode4(Arm& cpu)
	:AddressingMode(cpu)
{

}

AddrMode4Result AddressingMode4::incrementAfter(ArmInstruction& ins)
{
	AddrMode4Result result;
	result.writeback = false;

	u8 W = ins.W();
	RegisterID rn = ins.rn();
	u32 reg_rn = cpu.getRegister(rn);
	u16 reg_list = ins.registerList();

	u32 start_address = reg_rn;
	u32 end_address = reg_rn + ((numSetBitsU16(reg_list) * 4) - 4);

	result.startAddress = start_address;
	result.endAddress = end_address;

	//if W == 1, base register rn is updated after the transfer
	if (W == 0x1) {
		reg_rn = reg_rn + (numSetBitsU16(reg_list) * 4);
		result.rn = reg_rn;
		result.writeback = true;
	}

	return result;
}

AddrMode4Result AddressingMode4::incrementBefore(ArmInstruction& ins)
{
	AddrMode4Result result;
	result.writeback = false;

	u8 W = ins.W();
	RegisterID rn = ins.rn();
	u32 reg_rn = cpu.getRegister(rn);
	u16 reg_list = ins.registerList();

	u32 start_address = reg_rn + 4;
	u32 end_address = reg_rn + (numSetBitsU16(reg_list) * 4);

	result.startAddress = start_address;
	result.endAddress = end_address;

	if (W == 0x1) {
		reg_rn = reg_rn + (numSetBitsU16(reg_list) * 4);
		result.rn = reg_rn;
		result.writeback = true;
	}

	return result;
}

AddrMode4Result AddressingMode4::decrementAfter(ArmInstruction& ins)
{
	AddrMode4Result result;
	result.writeback = false;

	u8 W = ins.W();
	RegisterID rn = ins.rn();
	u32 reg_rn = cpu.getRegister(rn);
	u16 reg_list = ins.registerList();

	u32 start_address = reg_rn - ((numSetBitsU16(reg_list) * 4) + 4);
	u32 end_address = reg_rn;

	result.startAddress = start_address;
	result.endAddress = end_address;

	if (W == 0x1) {
		reg_rn = reg_rn - (numSetBitsU16(reg_list) * 4);
		result.rn = reg_rn;
		result.writeback = true;
	}

	return result;
}

AddrMode4Result AddressingMode4::decrementBefore(ArmInstruction& ins)
{
	AddrMode4Result result;
	result.writeback = false;

	u8 W = ins.W();
	RegisterID rn = ins.rn();
	u32 reg_rn = cpu.getRegister(rn);
	u16 reg_list = ins.registerList();

	u32 start_address = reg_rn - (numSetBitsU16(reg_list) * 4);
	u32 end_address = reg_rn - 4;

	result.startAddress = start_address;
	result.endAddress = end_address;

	if (W == 0x1) {
		reg_rn = reg_rn - (numSetBitsU16(reg_list) * 4);
		result.rn = reg_rn;
		result.writeback = true;
	}

	return result;
}


