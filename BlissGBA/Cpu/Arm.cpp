#include "Arm.h"
#include "../Memory/MemoryBus.h"

Arm::Arm(MemoryBus *mbus)
	:addrMode1(*this), addrMode2(*this),
	addrMode3(*this), addrMode4(*this)
{
	mapArmOpcodes();
	mapThumbOpcodes();

	this->mbus = mbus;
}

u8 Arm::clock()
{
	if (state == State::ARM) {
		currentExecutingArmOpcode = armpipeline[0];
		armpipeline[0] = armpipeline[1];
		
		ArmInstruction ins;
		ins.encoding = currentExecutingArmOpcode;
		executeArmIns(ins);

		armpipeline[1] = fetchU32(); 
	}
	else if (state == State::THUMB) {
		currentExecutingThumbOpcode = thumbpipeline[0];
		thumbpipeline[0] = thumbpipeline[1];
		
		ThumbInstruction ins;
		ins.encoding = currentExecutingThumbOpcode;
		executeThumbIns(ins);

		thumbpipeline[1] = fetchU16();
	}
	checkStateAndProcessorMode();

	return cycles;
}

void Arm::handleTimers()
{

}

void Arm::handleInterrupts()
{
	u8 interrupts = getFlag(I);
	if (interrupts == 0x0) {
		u8 interrupt_master = mbus->readU32(IME) & 0x1;
		if (interrupt_master) {
			u16 interrupt_enable = mbus->readU16(IE);
			u16 interrupt_req_flag = mbus->readU16(IF);
			//There is an interrupt to be serviced
			if (interrupt_enable & interrupt_req_flag) {
				//Save state before jumping
				if (getFlag(T) == 0x0) {
					//+ 4 because when returning after servicing an interrupt bios executes subs pc, r14, #4
					LR_irq = (R15 - 4) + 4; //next instruction + 4
				}
				else {
					LR_irq = (R15 - 2) + 2;
				}
				SPSR_irq = CPSR;

				enterIRQMode();
				clearFlag(T); //execute in arm state
				setFlag(I); //disable irqs

				R15 = IRQ_VECTOR;
				flushPipeline();
			}
		}
	}
}

void Arm::checkStateAndProcessorMode()
{
	mode = getProcessorMode();
	state = getState();
}

void Arm::reset()
{
	cycles = 0;
	for (s32 i = 0; i < NUM_REGISTERS; i++) registers[i].value = 0x0;
	for (s32 i = 0; i < NUM_REGISTERS_FIQ; i++) registersFiq[i].value = 0x0;
	
	//System/User
	LR = 0x00000000; //R14
	R15 = 0x08000000;
	SP = 0x03007F00; //R13
	CPSR = 0x000000DF;

	SPSR = CPSR;
	SPSR_irq = 0x0;
	SPSR_fiq = 0x0;
	SPSR_svc = 0x0;
	SPSR_abt = 0x0;
	SPSR_und = 0x0;

	SP_fiq = 0x0;
	SP_irq = 0x03007FA0;
	SP_svc = 0x03007FE0;
	SP_abt = 0x0;
	SP_und = 0x0;

	LR_fiq = 0x0;
	LR_irq = 0x0;
	LR_svc = 0x0;
	LR_abt = 0x0;
	LR_und = 0x0;

	mode = getProcessorMode();

	armpipeline[0] = fetchU32();
	armpipeline[1] = fetchU32();
}

void Arm::setFlag(u32 flagBits, bool condition)
{
	if (condition) {
		setFlag(flagBits);
	}
}

void Arm::setFlag(u32 flagBits)
{
	//M0 - M4
	for (s32 i = 0; i < 5; i++) {
		u8 mi = (1 << i);
		if (flagBits & mi) {
			CPSR |= mi;
		}
	}

	if (flagBits & T) {
		CPSR |= T;
	}
	if (flagBits & F) {
		CPSR |= F;
	}
	if (flagBits & I) {
		CPSR |= I;
	}
	if (flagBits & V) {
		CPSR |= V;
	}
	if (flagBits & C) {
		CPSR |= C;
	}
	if (flagBits & Z) {
		CPSR |= Z;
	}
	if (flagBits & N) {
		CPSR |= N;
	}
}

void Arm::clearFlag(u32 flagBits)
{
	//M0 - M4
	for (s32 i = 0; i < 5; i++) {
		u8 mi = (1 << i);
		if (flagBits & mi) {
			CPSR &= ~(mi);
		}
	}

	if (flagBits & T) {
		CPSR &= ~(T);
	}
	if (flagBits & F) {
		CPSR &= ~(F);
	}
	if (flagBits & I) {
		CPSR &= ~(I);
	}
	if (flagBits & V) {
		CPSR &= ~(V);
	}
	if (flagBits & C) {
		CPSR &= ~(C);
	}
	if (flagBits & Z) {
		CPSR &= ~(Z);
	}
	if (flagBits & N) {
		CPSR &= ~(N);
	}
}

void Arm::setFlagSPSR(u32 flagBits)
{
	//M0 - M4
	for (s32 i = 0; i < 5; i++) {
		u8 mi = (1 << i);
		if (flagBits & mi) {
			SPSR |= mi;
		}
	}

	if (flagBits & T) {
		SPSR |= T;
	}
	if (flagBits & F) {
		SPSR |= F;
	}
	if (flagBits & I) {
		SPSR |= I;
	}
	if (flagBits & V) {
		SPSR |= V;
	}
	if (flagBits & C) {
		SPSR |= C;
	}
	if (flagBits & Z) {
		SPSR |= Z;
	}
	if (flagBits & N) {
		SPSR |= N;
	}
}

void Arm::clearFlagSPSR(u32 flagBits)
{
	//M0 - M4
	for (s32 i = 0; i < 5; i++) {
		u8 mi = (1 << i);
		if (flagBits & mi) {
			SPSR &= ~(mi);
		}
	}

	if (flagBits & T) {
		SPSR &= ~(T);
	}
	if (flagBits & F) {
		SPSR &= ~(F);
	}
	if (flagBits & I) {
		SPSR &= ~(I);
	}
	if (flagBits & V) {
		SPSR &= ~(V);
	}
	if (flagBits & C) {
		SPSR &= ~(C);
	}
	if (flagBits & Z) {
		SPSR &= ~(Z);
	}
	if (flagBits & N) {
		SPSR &= ~(N);
	}
}

void Arm::fillPipeline()
{
	u32 first_instr = (mbus->readU8(R15)) | (mbus->readU8(R15 + 1) << 8) |
		(mbus->readU8(R15 + 2) << 16) | (mbus->readU8(R15 + 3) << 24);

	u32 second_instr = (mbus->readU8(R15 + 4)) | (mbus->readU8(R15 + 5) << 8) |
		(mbus->readU8(R15 + 6) << 16) | (mbus->readU8(R15 + 7) << 24);

	//Fill pipeline at boot with first 2 instructions
	armpipeline[0] = first_instr;
	armpipeline[1] = second_instr;
}

void Arm::flushPipeline() 
{
	armpipeline[0] = fetchU32();
	armpipeline[1] = readU32();
}

void Arm::flushThumbPipeline()
{
	thumbpipeline[0] = fetchU16();
	thumbpipeline[1] = readU16();
}

void Arm::enterSupervisorMode()
{
	clearFlag(M2 | M3);
	setFlag(M0 | M1 | M4);
}

void Arm::enterIRQMode()
{
	clearFlag(M0 | M2 | M3);
	setFlag(M1 | M4);
}

u8 Arm::getFlag(u32 flag)
{
	return (CPSR & flag) ? 1 : 0;
}

u8 Arm::carryFrom(u32 op1, u32 op2)
{
	return (((u64)op1 + (u64)op2) > 0xFFFFFFFF);
}

u8 Arm::borrowFrom(u32 op1, u32 op2)
{
	return (((s64)op1 - (s64)op2) < 0);
}

u8 Arm::overflowFromAdd(u32 op1, u32 op2)
{
	s64 result = op1 + op2;
	return (~(op1 ^ op2) & ((op1 ^ result)) & 0x80000000) != 0;
}

u8 Arm::overflowFromSub(u32 op1, u32 op2) 
{
	s64 result = op1 - op2;
	return ((op1 ^ op2) & ((op1 ^ result)) & 0x80000000) != 0;
}

u8 Arm::fetchOp(u32 encoding)
{
	u8 opcode = ((encoding >> 21) & 0xF);
	return opcode;
}

PSR Arm::getPSR()
{
	return PSR{
		CPSR,
		SPSR
	};
}

u32 Arm::getSPSR()
{
	if (mode == ProcessorMode::USER || mode == ProcessorMode::SYS) {
		return CPSR;
	}
	else if (mode == ProcessorMode::FIQ) {
		return SPSR_fiq;
	}
	else if (mode == ProcessorMode::IRQ) {
		return SPSR_irq;
	}
	else if (mode == ProcessorMode::SVC) {
		return SPSR_svc;
	}
	else if (mode == ProcessorMode::ABT) {
		return SPSR_abt;
	}
	else if (mode == ProcessorMode::UND) {
		return SPSR_und;
	}
}

u32 Arm::getRegister(RegisterID reg)
{
	//Registers r0 - r7 remain the same for all states
	if (reg.id <= 7) {
		return registers[reg.id].value;
	}
	else {
		if (mode == ProcessorMode::USER || mode == ProcessorMode::SYS) {
			if (reg.id >= R13_ID) {
				if (reg.id == R13_ID) return SP;
				if (reg.id == R14_ID) return LR;
				if (reg.id == R15_ID) return R15;
			}
			else {
				//return registers within r8 - r12 range
				return registers[reg.id].value;
			}
		}
		else if (mode == ProcessorMode::FIQ) {
			if (reg.id >= R13_ID) {
				if (reg.id == R13_ID) return SP_fiq;
				if (reg.id == R14_ID) return LR_fiq;
				if (reg.id == R15_ID) return R15;
			}
			else {
				u8 index = reg.id - 8;
				if (index == NUM_REGISTERS_FIQ) index -= 1; //array out of bounds check
				return registersFiq[index].value;
			}
		}
		else if (mode == ProcessorMode::IRQ) {
			if (reg.id >= R13_ID) {
				if (reg.id == R13_ID) return SP_irq;
				if (reg.id == R14_ID) return LR_irq;
				if (reg.id == R15_ID) return R15;
			}
			else {
				return registers[reg.id].value;
			}
		}
		else if (mode == ProcessorMode::SVC) {
			if (reg.id >= R13_ID) {
				if (reg.id == R13_ID) return SP_svc;
				if (reg.id == R14_ID) return LR_svc;
				if (reg.id == R15_ID) return R15;
			}
			else {
				return registers[reg.id].value;
			}
		}
		else if (mode == ProcessorMode::ABT) {
			if (reg.id >= R13_ID) {
				if (reg.id == R13_ID) return SP_abt;
				if (reg.id == R14_ID) return LR_abt;
				if (reg.id == R15_ID) return R15;
			}
			else {
				return registers[reg.id].value;
			}
		}
		else if (mode == ProcessorMode::UND) {
			if (reg.id >= R13_ID) {
				if (reg.id == R13_ID) return SP_und;
				if (reg.id == R14_ID) return LR_und;
				if (reg.id == R15_ID) return R15;
			}
			else {
				return registers[reg.id].value;
			}
		}
	}
}

u32 Arm::getUserModeRegister(RegisterID reg)
{
	if (reg.id >= R13_ID) {
		if (reg.id == R13_ID) return SP;
		if (reg.id == R14_ID) return LR;
		if (reg.id == R15_ID) return R15;
	}
	else {
		return registers[reg.id].value;
	}
}

void Arm::writeRegister(RegisterID reg, u32 value)
{
	//Registers r0 - r7 remain the same for all states
	if (reg.id <= 7) {
		registers[reg.id].value = value;
	}
	else {
		if (mode == ProcessorMode::USER || mode == ProcessorMode::SYS) {
			if (reg.id >= R13_ID) {
				if (reg.id == R13_ID) SP = value;
				if (reg.id == R14_ID) LR = value;
				if (reg.id == R15_ID) R15 = value;
			}
			else {
				//modify registers within r8 - r12 range
				registers[reg.id].value = value;
			}
		}
		else if (mode == ProcessorMode::FIQ) {
			if (reg.id >= R13_ID) {
				if (reg.id == R13_ID) SP_fiq = value;;
				if (reg.id == R14_ID) LR_fiq = value;
				if (reg.id == R15_ID) R15 = value;
			}
			else {
				u8 index = reg.id - 8;
				if (index == NUM_REGISTERS_FIQ) index -= 1;
				registersFiq[index].value = value;
			}
		}
		else if (mode == ProcessorMode::IRQ) {
			if (reg.id >= R13_ID) {
				if (reg.id == R13_ID) SP_irq = value;
				if (reg.id == R14_ID) LR_irq = value;
				if (reg.id == R15_ID) R15 = value;
			}
			else {
				registers[reg.id].value = value;
			}
		}
		else if (mode == ProcessorMode::SVC) {
			if (reg.id >= R13_ID) {
				if (reg.id == R13_ID) SP_svc = value;
				if (reg.id == R14_ID) LR_svc = value;
				if (reg.id == R15_ID) R15 = value;
			}
			else {
				registers[reg.id].value = value;
			}
		}
		else if (mode == ProcessorMode::ABT) {
			if (reg.id >= R13_ID) {
				if (reg.id == R13_ID) SP_abt = value;
				if (reg.id == R14_ID) LR_abt = value;
				if (reg.id == R15_ID) R15 = value;
			}
			else {
				registers[reg.id].value = value;
			}
		}
		else if (mode == ProcessorMode::UND) {
			if (reg.id >= R13_ID) {
				if (reg.id == R13_ID) SP_und = value;
				if (reg.id == R14_ID) LR_und = value;
				if (reg.id == R15_ID) R15 = value;
			}
			else {
				registers[reg.id].value = value;
			}
		}
	}
}

void Arm::writeUserModeRegister(RegisterID reg, u32 value)
{
	if (reg.id >= R13_ID) {
		if (reg.id == R13_ID) SP = value;
		if (reg.id == R14_ID) LR = value;
		if (reg.id == R15_ID) R15 = value;
	}
	else {
		registers[reg.id].value = value;
	}
}

void Arm::writeSPSR(u32 spsr)
{
	if (mode == ProcessorMode::FIQ) {
		SPSR_fiq = spsr;
	}
	else if (mode == ProcessorMode::IRQ) {
		SPSR_irq = spsr;
	}
	else if (mode == ProcessorMode::SVC) {
		SPSR_svc = spsr;
	}
	else if (mode == ProcessorMode::ABT) {
		SPSR_abt = spsr;
	}
	else if (mode == ProcessorMode::UND) {
		SPSR_und = spsr;
	}
}

void Arm::writePC(u32 pc)
{
	R15 |= ((pc << 2) & 0xFFFFFFFF);
}

void Arm::setCC(u32 result, RegisterID rd, bool borrow, bool overflow,
	bool shiftOut, u8 shifterCarryOut)
{
	if (rd.id != R15_ID) {
		(result >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
		(result == 0) ? setFlag(Z) : clearFlag(Z);
		if (shiftOut) {
			//overflow not affected
			(shifterCarryOut == 1) ? setFlag(C) : clearFlag(C);
		}
		else {
			(borrow == false) ? setFlag(C) : clearFlag(C);
			(overflow == true) ? setFlag(V) : clearFlag(V);
		}
	}
	else {
		CPSR = getSPSR();
	}
}

bool Arm::inPrivilegedMode()
{
	return ((mode == ProcessorMode::SYS) ||
		(mode == ProcessorMode::IRQ) ||
		(mode == ProcessorMode::FIQ) ||
		(mode == ProcessorMode::SVC) ||
		(mode == ProcessorMode::ABT) ||
		(mode == ProcessorMode::UND));
}

bool Arm::currentModeHasSPSR()
{
	return ((mode == ProcessorMode::FIQ) ||
		(mode == ProcessorMode::IRQ) ||
		(mode == ProcessorMode::SVC) ||
		(mode == ProcessorMode::ABT) ||
		(mode == ProcessorMode::UND));
}

ProcessorMode Arm::getProcessorMode()
{
	ProcessorMode mode = ProcessorMode::SYS;
	u8 mbits = CPSR & 0x1F;
	switch (mbits) {
		case 0b10000: mode = ProcessorMode::USER; break;
		case 0b10001: mode = ProcessorMode::FIQ; break;
		case 0b10010: mode = ProcessorMode::IRQ; break;
		case 0b10011: mode = ProcessorMode::SVC; break;
		case 0b10111: mode = ProcessorMode::ABT; break;
		case 0b11011: mode = ProcessorMode::UND; break;
		case 0b11111: mode = ProcessorMode::SYS; break;
	}
	return mode;
}

State Arm::getState()
{
	State cs = (State)getFlag(T);
	return cs;
}

u8 Arm::getConditionCode(u8 cond)
{
	switch (cond) {
		case 0b0000: return getFlag(Z); break;
		case 0b0001: return (getFlag(Z) == 0); break;
		case 0b0010: return getFlag(C); break;
		case 0b0011: return (getFlag(C) == 0); break;
		case 0b0100: return getFlag(N); break;
		case 0b0101: return (getFlag(N) == 0); break;
		case 0b0110: return getFlag(V); break;
		case 0b0111: return (getFlag(V) == 0); break;
		case 0b1000: return (getFlag(C) && (getFlag(Z) == 0)); break;
		case 0b1001: return ((getFlag(C) == 0) || getFlag(Z)); break;
		case 0b1010: return (getFlag(N) == getFlag(V)); break;
		case 0b1011: return (getFlag(N) != getFlag(V)); break;
		case 0b1100: return ((getFlag(Z) == 0) && (getFlag(N) == getFlag(V))); break;
		case 0b1101: return (getFlag(Z) || (getFlag(N) != getFlag(V))); break;
		case 0b1110: return 1; break;//always
		default: return 1; break; //always
	}
}

void Arm::writeU16(u32 address, u16 value)
{
	//Cpu only, write to IF means the program is
	//trying to clear a bit
	if (address == IF) {
		u16 irq_flag = mbus->readU16(IF);
		irq_flag &= ~(value);
		mbus->writeU16(IF, irq_flag);
	}
	else
		mbus->writeU16(address, value);
}

u16 Arm::readU16()
{
	u16 halfword = (mbus->readU8(R15)) |
		(mbus->readU8(R15 + 1) << 8);

	return halfword;
}

u16 Arm::fetchU16()
{
	u16 halfword = (mbus->readU8(R15++)) |
		(mbus->readU8(R15++) << 8);

	return halfword;
}

u32 Arm::readU32()
{
	u32 word = (mbus->readU8(R15)) |
		(mbus->readU8(R15 + 1) << 8) |
		(mbus->readU8(R15 + 2) << 16) |
		(mbus->readU8(R15 + 3) << 24);

	return word;
}

u32 Arm::fetchU32()
{
	u32 word = (mbus->readU8(R15++)) |
		(mbus->readU8(R15++) << 8) |
		(mbus->readU8(R15++) << 16) |
		(mbus->readU8(R15++) << 24);

	return word;
}

u32 Arm::shift(u32 value, u8 amount, u8 type, u8 &shiftedBit, bool immediate)
{
	switch (type) {
		case 0b00:
			value = lsl(value, amount, shiftedBit, immediate);
			break;

		case 0b01:
			value = lsr(value, amount, shiftedBit, immediate);
			break;

		case 0b10:
			value = asr(value, amount, shiftedBit, immediate);
			break;

		case 0b11:
			value = ror(value, amount, shiftedBit, immediate);
			break;
	}
	return value;
}

u32 Arm::lsl(u32 value, u8 shift, u8& shiftedBit, bool immediate)
{
	u32 result = 0;
	if (immediate) {
		if (shift == 0) {
			result = value;
			shiftedBit = getFlag(C);
		}
		else { //shift > 0
			shiftedBit = (32 - (shift));
			shiftedBit = ((value >> shiftedBit) & 0x1);
			
			result = value << shift;
		}
	}
	else {
		if (shift == 0) {
			result = value;
			shiftedBit = getFlag(C);
		}
		else if (shift < 32) {
			if (shift != 0) {
				//Save last carried out bit
				shiftedBit = (32 - (shift));
				shiftedBit = ((value >> shiftedBit) & 0x1);
			}
			result = value << shift;
		}
		else if(shift == 32) {
			result = 0;
			shiftedBit = value & 0x1;
		}
		else { //shift > 32
			result = 0;
			shiftedBit = 0;
		}
	}
	return result;
}

u32 Arm::lsr(u32 value, u8 shift, u8& shiftedBit, bool immediate)
{
	u32 result = 0;
	if (immediate) {
		//shift == 0 encoded as 32
		if (shift == 0) {
			result = 0;
			shiftedBit = (value >> 31) & 0x1;
		}
		//shift > 0
		else {
			shiftedBit = (shift - 1);
			shiftedBit = ((value >> shiftedBit) & 0x1);

			result = value >> shift;
			if (shift > 32) {
				result = 0;
			}
		}
	}
	//Register shift
	else {
		//zero shift
		if (shift == 0) {
			result = value;
			shiftedBit = getFlag(C);
		}
		else if (shift < 32) {
			if (shift != 0) {
				shiftedBit = (shift - 1);
				shiftedBit = ((value >> shiftedBit) & 0x1);
			}
			result = value >> shift;
		}
		else if (shift == 32) {
			result = 0;
			shiftedBit = (value >> 31) & 0x1;
		}
		//shift > 32
		else {
			result = 0;
			shiftedBit = 0;
		}
	}
	return result;
}

u32 Arm::asr(u32 value, u8 shift, u8& shiftedBit, bool immediate)
{
	u32 result = 0;
	if (immediate) {
		//shift == 0 encoded as 32
		if (shift == 0) {
			u8 sign_bit_rm = (value >> 31) & 0x1;
			if (sign_bit_rm == 0) {
				result = 0;
				shiftedBit = sign_bit_rm;
			}
			else { //rm[31] == 1
				result = 0xFFFFFFFF;
				shiftedBit = sign_bit_rm;
			}
		}
		//shift > 0
		else {
			if (shift == 31) {
				if ((value >> 31) & 0x1) {
					result = 0xFFFFFFFF;
					shiftedBit = (value >> 31) & 0x1;

					return result;
				}
			}

			shiftedBit = (shift - 1);
			shiftedBit = ((value >> shiftedBit) & 0x1);

			result = value >> shift;
			result = signExtend32(result, 32 - shift);
		}
	}
	//Register shift
	else {
		//zero shift
		if (shift == 0) {
			result = value;
			shiftedBit = getFlag(C);
		}
		else if (shift < 32) {
			shiftedBit = (shift - 1);
			shiftedBit = ((value >> shiftedBit) & 0x1);

			result = value >> shift;
			result = signExtend32(result, 32 - shift);
		}
		//shift >= 32
		else {
			u8 sign_bit_rm = (value >> 31) & 0x1;
			if (sign_bit_rm == 0) {
				result = 0;
				shiftedBit = (value >> 31) & 0x1;
			}
			else {
				result = 0xFFFFFFFF;
				shiftedBit = (value >> 31) & 0x1;
			}
		}
	}
	return result;
}

u32 Arm::ror(u32 value, u8 shift, u8 &shiftedBit, bool immediate)
{
	u32 result = 0;
	if (immediate) {
		if (shift == 0) {
			result = rrx(value, shiftedBit);
			return result;
		}
		else {
			u32 rotatedOut = getNthBits(value, 0, shift);
			u32 rotatedIn = getNthBits(value, shift, 31);

			shiftedBit = (shift - 1);
			shiftedBit = ((value >> shiftedBit) & 0x1);

			result = value >> shift;
			result |= (rotatedOut << (32 - shift));
		}
	}
	else {
		//Register shift
		if (shift == 0) {
			result = value;
			shiftedBit = getFlag(C);
		}
		else if ((shift & 0x1F) == 0) {
			result = value;
			shiftedBit = (value >> 31) & 0x1;
		}
		//rm[0-4] > 0
		else {
			u32 rotatedOut = getNthBits(value, 0, shift);
			u32 rotatedIn = getNthBits(value, shift, 31);

			shiftedBit = (shift - 1);
			shiftedBit = ((value >> shiftedBit) & 0x1);

			result = value >> shift;
			result |= (rotatedOut << (32 - shift));
		}
	}
	return result;
}

u32 Arm::ror(u32 value, u8 shift)
{
	u32 rotatedOut = getNthBits(value, 0, shift);
	u32 rotatedIn = getNthBits(value, shift, 31);

	value >>= shift;
	value |= (rotatedOut << (32 - shift));
	
	return value;
}

u32 Arm::rrx(u32 value, u8 &shiftedBit)
{
	//shifter_carry_out
	shiftedBit = value & 0x1;
	value = (getFlag(C) << 31) | (value >> 1);
	
	return value;
}

void Arm::executeArmIns(ArmInstruction& ins)
{
	u8 cond = getConditionCode(ins.cond());
	if (!cond) {
		cycles = 1;
		return;
	}
	
	u16 instruction = ins.instruction();
	cycles = armlut[instruction](ins);
}

void Arm::executeThumbIns(ThumbInstruction& ins)
{
	u8 instruction = ins.instruction();
	cycles = thumblut[instruction](ins);
}

u8 Arm::executeDataProcessing(ArmInstruction& ins, bool flags, bool immediate)
{
	u8 hi_bits = (ins.encoding >> 24) & 0xF;
	u8 lo_bits = (ins.encoding >> 4) & 0xF;
	//Arithmetic instruction extension space
	if (hi_bits == 0x0 && lo_bits == 0b1001) {
		printf("arithmetic instruction set extension\n");
	}
	
	RegisterID rd = ins.rd();
	RegisterID rn = ins.rn();

	switch (ins.opcode()) {
		case 0b0000: return opAND(ins, rd, rn, flags, immediate); break;
		case 0b0001: return opEOR(ins, rd, rn, flags, immediate); break;
		case 0b0010: return opSUB(ins, rd, rn, flags, immediate); break;
		case 0b0011: return opRSB(ins, rd, rn, flags, immediate); break;
		case 0b0100: return opADD(ins, rd, rn, flags, immediate); break;
		case 0b0101: return opADC(ins, rd, rn, flags, immediate); break;
		case 0b0110: return opSBC(ins, rd, rn, flags, immediate); break;
		case 0b0111: return opRSC(ins, rd, rn, flags, immediate); break;
		case 0b1000: return opTST(ins, rd, rn, flags, immediate); break;
		case 0b1001: return opTEQ(ins, rd, rn, flags, immediate); break;
		case 0b1010: return opCMP(ins, rd, rn, flags, immediate); break;
		case 0b1011: return opCMN(ins, rd, rn, flags, immediate); break;
		case 0b1100: return opORR(ins, rd, rn, flags, immediate); break;
		case 0b1101: return opMOV(ins, rd, rn, flags, immediate); break;
		case 0b1110: return opBIC(ins, rd, rn, flags, immediate); break;
		case 0b1111: return opMVN(ins, rd, rn, flags, immediate); break;
	}

	return 1;
}

u8 Arm::executeDataProcessingImmFlags(ArmInstruction& ins)
{
	executeDataProcessing(ins, true, true);
	return 1;
}

u8 Arm::executeDataProcessingImm(ArmInstruction& ins)
{
	executeDataProcessing(ins, false, true);
	return 1;
}

u8 Arm::executeDataProcessingImmShiftFlags(ArmInstruction& ins)
{
	executeDataProcessing(ins, true, false);
	return 1;
}

u8 Arm::executeDataProcessingImmShift(ArmInstruction& ins)
{
	executeDataProcessing(ins, false, false);
	return 1;
}

u8 Arm::executeDataProcessingRegShiftFlags(ArmInstruction& ins)
{
	executeDataProcessing(ins, true, false);
	return 1;
}

u8 Arm::executeDataProcessingRegShift(ArmInstruction& ins)
{
	executeDataProcessing(ins, false, false);
	return 1;
}

u8 Arm::handleUndefinedIns(ArmInstruction& ins)
{
	u16 lutIndex = ins.instruction();
	printf("LUT Index: %d ", lutIndex);
	printf("ARM undefined or unimplemented instruction 0x%08X at PC: 0x%08X\n", ins.encoding, R15 - 8);

	return 0;
}

u8 Arm::executeLoadStore(ArmInstruction& ins, AddrModeLoadStoreResult& result)
{
	RegisterID rn = ins.rn();
	RegisterID rd = ins.rd();

	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	bool byte = (ins.B() == 0x1); //true == byte, false == word (transfer quantity)
	bool load = (ins.L() == 0x1); //load or store

	//Modify before
	if (result.type == AddrModeLoadStoreType::PREINDEXED) {
		reg_rn = result.address;
	}

	u32 address = result.address;
	if (load) {
		//LDR, LDRB
		if (byte) {
			opLDRB(ins, rd, address);
		}
		else {
			opLDR(ins, rd, address);
		}
	}
	else {
		//STR, STRB
		if (byte) {
			opSTRB(ins, rd, address);
		}
		else {
			opSTR(ins, rd, address);
		}
	}

	//Modify after
	if (result.type == AddrModeLoadStoreType::POSTINDEX) {
		reg_rn = result.rn;
	}

	//Both write back after
	if (result.type == AddrModeLoadStoreType::PREINDEXED ||
		result.type == AddrModeLoadStoreType::POSTINDEX) {
		//Don't write back if reg base == reg dest when ldr
		if (load && (rd.id == rn.id)) return 1;

		writeRegister(rn, reg_rn);
	}

	return 1;
}

u8 Arm::executeLoadStoreImm(ArmInstruction& ins)
{
	AddrModeLoadStoreResult result = addrMode2.immOffsetIndex(ins);
	executeLoadStore(ins, result);

	return 1;
}

u8 Arm::executeLoadStoreShift(ArmInstruction& ins)
{
	AddrModeLoadStoreResult result = addrMode2.scaledRegisterOffsetIndex(ins);
	executeLoadStore(ins, result);

	return 1;
}

u8 Arm::executeMiscLoadAndStore(ArmInstruction& ins, AddrModeLoadStoreResult &result)
{
	RegisterID rn = ins.rn();
	RegisterID rd = ins.rd();

	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	bool load = (ins.L() == 0x1);
	u8 SH = (ins.S() << 1) | ins.H();

	//Modify before
	if (result.type == AddrModeLoadStoreType::PREINDEXED) {
		reg_rn = result.address;
	}

	u32 address = result.address; 
	switch (SH) {
		case 0b00: { //SWP, SWPB
			u8 b22 = (ins.encoding >> 22) & 0x1;
			(b22 == 0x0) ? opSWP(ins, rd, rn) : opSWPB(ins, rd, rn);
		}
		break;
		case 0b01: //Unsigned halfwords
		{
			if (!load) {
				opSTRH(ins, rd, address);
			}
			else {
				opLDRH(ins, rd, address);
			}
		}
		break;

		case 0b10: //Signed byte
		{
			if (!load) {
				opSTRH(ins, rd, address);
			}
			else { 
				opLDRSB(ins, rd, address);
			}
		}
		break;

		case 0b11: //Signed halfword
		{
			if (!load) {
				opSTRH(ins, rd, address);
			}
			else {
				opLDRSH(ins, rd, address);
			}
		}
		break;
	}

	//Modify after
	if (result.type == AddrModeLoadStoreType::POSTINDEX) {
		reg_rn = result.rn;
	}

	//Both write back after
	if (result.type == AddrModeLoadStoreType::PREINDEXED ||
		result.type == AddrModeLoadStoreType::POSTINDEX) {
		//Don't write back if reg base == reg dest when ldr
		if (load && (rd.id == rn.id)) return 1;

		writeRegister(rn, reg_rn);
	}

	return 1;
}

u8 Arm::executeMiscLoadStoreImm(ArmInstruction& ins)
{
	AddrModeLoadStoreResult result = addrMode3.immOffsetIndex(ins);
	executeMiscLoadAndStore(ins, result);

	return 1;
}

u8 Arm::executeMiscLoadStoreReg(ArmInstruction& ins)
{
	AddrModeLoadStoreResult result = addrMode3.registerOffsetIndex(ins);
	executeMiscLoadAndStore(ins, result);

	return 1;
}

u8 Arm::executeLDM(ArmInstruction& ins)
{
	u8 PU = (ins.P() << 1) | ins.U();
	u8 S = ins.S_bit22();
	AddrMode4Result result;
	bool incrementing;
	bool ia = false;
	bool ib = false;
	bool da = false;
	bool db = false;

	switch (PU) {
		case 0b01: {
			ia = true;
			incrementing = true;
			result = addrMode4.incrementAfter(ins);
		}
		break;
		case 0b11: {
			ib = true;
			incrementing = true;
			result = addrMode4.incrementBefore(ins);
		}
		break;
		case 0b00: {
			da = true;
			incrementing = false;
			result = addrMode4.decrementAfter(ins);
		}
		break;
		case 0b10: {
			db = true;
			incrementing = false;
			result = addrMode4.decrementBefore(ins);
		}
		break;
	}

	RegisterID rn = ins.rn();
	u16 reg_list = ins.registerList();
	u32 address = result.startAddress;
	u32 end = result.endAddress;

	address &= 0xFFFFFFFC;


	//Check if reg base is in the list. If it is we don't writeback
	bool rb_in_list = false;
	for (s32 i = 0; i <= 7; i++) {
		if (testBit(reg_list, i) && rn.id == i) {
			rb_in_list = true;
			break;
		}
	}

	//Empty list
	if (reg_list == 0x0) {
		//Load value from reg_rn address into r15
		u32 addr = mbus->readU32(address);
		R15 = addr & 0xFFFFFFFC;
		flushPipeline();

		if (incrementing) result.rn += 0x40;
		else result.rn -= 0x40;
	}

	bool load_pc = testBit(reg_list, 15);
	for (s32 i = 0; i <= 14; i++) {
		bool included = testBit(reg_list, i);
		if (included) {
			RegisterID id; id.id = i;
			u32 value = mbus->readU32(address);
			//Load user mode registers
			if (S == 0x1 && !load_pc) {
				writeUserModeRegister(id, value);
			}
			//Load registers of current mode
			else {
				writeRegister(id, value);
			}
			address += 4;
		}

		if (end == address - 4) break;
	}

	//R15
	if (load_pc) {
		u32 value = mbus->readU32(address);
		R15 = value & 0xFFFFFFFC;
		flushPipeline();

		if (S == 0x1) {
			CPSR = getSPSR();
		}
	}

	if (result.writeback) {
		//No writeback if rb is first in rlist or if it is in the list at all
		if (!rb_in_list)
			writeRegister(rn, result.rn);
	}

	return 1;
}

u8 Arm::executeSTM(ArmInstruction& ins)
{
	u8 PU = (ins.P() << 1) | ins.U();
	u8 S = ins.S_bit22();
	AddrMode4Result result;
	bool incrementing;
	bool ia = false;
	bool ib = false;
	bool da = false;
	bool db = false;

	switch (PU) {
		case 0b01: {
			ia = true;
			incrementing = true;
			result = addrMode4.incrementAfter(ins);
		}
		break;
		case 0b11: { 
			ib = true;
			incrementing = true;
			result = addrMode4.incrementBefore(ins);
		} 
		break;
		case 0b00: {
			da = true;
			incrementing = false;
			result = addrMode4.decrementAfter(ins); 
		}
		break;
		case 0b10: {
			db = true;
			incrementing = false;
			result = addrMode4.decrementBefore(ins); 
		}
		break;
	}

	RegisterID rn = ins.rn();
	u16 reg_list = ins.registerList();
	u32 address = result.startAddress;
	u32 end = result.endAddress;

	address &= 0xFFFFFFFC;


	//Check if first reg in list is rb
	s8 first_reg = -1;
	for (s32 i = 0; i <= 7; i++) {
		if (testBit(reg_list, i)) {
			first_reg = i;
			break;
		}
	}

	//Is rb the first register in the list?
	bool first = false;
	if (rn.id == first_reg) {
		first = true;
	}

	bool rb_in_list = false;
	//Empty list
	if (reg_list == 0x0) {
		u32 base_addr;
		if (ib) base_addr = result.rn + 0x4;
		if (ia) base_addr = result.rn + 0x0;
		if (db) base_addr = result.rn - 0x40;
		if (da) base_addr = result.rn - 0x3C;

		mbus->writeU32(base_addr, R15 + 4);

		if (incrementing) result.rn += 0x40;
		else result.rn -= 0x40;

		if (result.writeback) writeRegister(rn, result.rn);

		return 1;
	}
	else { //Populated list
		//check is rb is in rlist if not first
		for (s32 i = 0; i <= 7; i++) {
			if (testBit(reg_list, i)) {
				rb_in_list = true;
				break;
			}
		}
	}

	for (s32 i = 0; i <= 14; i++) {
		bool included = testBit(reg_list, i);
		if (included) {
			RegisterID id; id.id = i;

			//Store user mode registers only
			if (S == 0x1) {
				u32 reg_usr = getUserModeRegister(id);
				mbus->writeU32(address, reg_usr);
			}
			//Store registers of current mode
			else {
				u32 reg = getRegister(id);
				mbus->writeU32(address, reg);
			}
			address += 4;

			//writeback old base if rb first in list
			if (first) {
				u32 reg_rn = getRegister(rn);
				writeRegister(rn, reg_rn);
				first = false;
			}
			else if (rb_in_list) { //Not first but rb is in the list
				//Store after first transfer occured
				if (result.writeback) {
					writeRegister(rn, result.rn);
					rb_in_list = false;
				}
			}
		}

		if (end == address - 4) break;
	}

	bool store_pc = testBit(reg_list, 15);
	//If R15 is to be stored, store 4 ahead (R15 + 4) to memory
	if (store_pc) {
		mbus->writeU32(address, R15 + 4);
	}

	if (result.writeback) {
		writeRegister(rn, result.rn);
	}

	return 1;
}

u8 Arm::executeMSRImm(ArmInstruction& ins)
{
	u8 immediate = ins.imm();
	u8 rotate_imm = ins.rotate();
	u32 operand = ror(immediate, (rotate_imm * 2));

	opMSR(ins, operand);

	return 1;
}

u8 Arm::executeMSRReg(ArmInstruction& ins)
{
	RegisterID rm = ins.rm();
	u32 operand = getRegister(rm);
	opMSR(ins, operand);

	return 1;
}

u8 Arm::executeSwap(ArmInstruction& ins)
{
	RegisterID rd = ins.rd();
	RegisterID rn = ins.rn();
	u8 b22 = (ins.encoding >> 22) & 0x1;

	if (b22 == 0x0)
		opSWP(ins, rd, rn);
	else
		opSWPB(ins, rd, rn);

	return 1;
}

u8 Arm::executeMultiplyLong(ArmInstruction& ins)
{
	u8 mull = (ins.encoding >> 21) & 0x7;
	bool flags = ins.s();

	u8 rdhi_id = (ins.encoding >> 16) & 0xF;
	u8 rdlo_id = (ins.encoding >> 12) & 0xF;
	u8 rs_id = (ins.encoding >> 8) & 0xF;
	u8 rm_id = (ins.encoding) & 0xF;

	RegisterID rdhi; rdhi.id = rdhi_id;
	RegisterID rdlo; rdlo.id = rdlo_id;
	RegisterID rs; rs.id = rs_id;
	RegisterID rm; rm.id = rm_id;

	switch (mull) {
		case 0b100: opUMULL(ins, rdhi, rdlo, rm, rs, flags); break;
		case 0b101: opUMLAL(ins, rdhi, rdlo, rm, rs, flags); break;
		case 0b110: opSMULL(ins, rdhi, rdlo, rm, rs, flags); break;
		case 0b111: opSMLAL(ins, rdhi, rdlo, rm, rs, flags); break;
	}

	return 1;
}

u8 Arm::executeMultiply(ArmInstruction& ins)
{
	u8 mul = (ins.encoding >> 21) & 0x1;
	bool flags = ins.s();

	u8 rd_id = (ins.encoding >> 16) & 0xF;
	u8 rm_id = (ins.encoding) & 0xF;
	u8 rs_id = (ins.encoding >> 8) & 0xF;

	if (mul == 0x0)
		opMUL(ins, RegisterID{ rd_id }, RegisterID{ rm_id }, RegisterID{ rs_id }, flags);
	else
		opMLA(ins, RegisterID{ rd_id }, RegisterID{ rm_id }, RegisterID{ rs_id }, flags);

	return 1;
}

u8 Arm::executeThumbUnconditionalBranch(ThumbInstruction& ins)
{
	u8 H = ins.H();
	if (H == 0b00)
		thumbOpB(ins);
	else
		thumbOpBL(ins);

	return 1;
}

u8 Arm::executeThumbLoadFromPool(ThumbInstruction& ins)
{
	thumbOpLDRPool(ins);

	return 1;
}

u8 Arm::executeThumbLoadStoreRegisterOffset(ThumbInstruction& ins)
{
	RegisterID rm = ins.rmUpper();
	RegisterID rn = ins.rnMiddle();
	RegisterID rd = ins.rdLower();

	u8 opcode = ins.opcode3();
	switch (opcode) {
		case 0b000: thumbOpSTR(ins, rm, rn, rd); break;
		case 0b001: thumbOpSTRH(ins, rm, rn, rd); break;
		case 0b010: thumbOpSTRB(ins, rm, rn, rd); break;
		case 0b011: thumbOpLDRSB(ins, rm, rn, rd); break;
		case 0b100: thumbOpLDR(ins, rm, rn, rd); break;
		case 0b101: thumbOpLDRH(ins, rm, rn, rd); break;
		case 0b110: thumbOpLDRB(ins, rm, rn, rd); break;
		case 0b111: thumbOpLDRSH(ins, rm, rn, rd); break;
	}

	return 1;
}

u8 Arm::executeThumbLoadStoreWordByteImmOffset(ThumbInstruction& ins)
{
	RegisterID rn = ins.rnMiddle();
	RegisterID rd = ins.rdLower();
	u8 imm5 = ins.imm5();

	u8 bl = (ins.B() << 1) | ins.L();
	switch (bl) {
		case 0b00: thumbOpSTR(ins, rn, rd, imm5); break;
		case 0b01: thumbOpLDR(ins, rn, rd, imm5); break;
		case 0b10: thumbOpSTRB(ins, rn, rd, imm5); break;
		case 0b11: thumbOpLDRB(ins, rn, rd, imm5); break;
	}

	return 1;
}

u8 Arm::executeThumbLoadStoreHalfwordImmOffset(ThumbInstruction& ins)
{
	bool load = (ins.L() == 0x1);

	RegisterID rn = ins.rnMiddle();
	RegisterID rd = ins.rdLower();
	u8 imm5 = ins.imm5();

	if (load) {
		thumbOpLDRH(ins, rn, rd, imm5);
	}
	else {
		thumbOpSTRH(ins, rn, rd, imm5);
	}

	return 1;
}

u8 Arm::executeThumbShiftByImm(ThumbInstruction& ins)
{
	u8 opcode = ins.opcode2();
	switch (opcode) {
		case 0b00: thumbOpLSL(ins, ins.imm5()); break;
		case 0b01: thumbOpLSR(ins, ins.imm5()); break;
		case 0b10: thumbOpASR(ins, ins.imm5()); break;
	}

	return 1;
}

u8 Arm::executeThumbDataProcessingImm(ThumbInstruction& ins)
{
	u8 opcode = ins.opcode2();
	switch (opcode) {
		case 0b00: thumbOpMOV(ins, ins.imm8()); break;
		case 0b01: thumbOpCMP(ins, ins.imm8()); break;
		case 0b10: thumbOpADD(ins, ins.rdUpper(), ins.imm8()); break;
		case 0b11: thumbOpSUB(ins, ins.rdUpper(), ins.imm8()); break;
	}

	return 1;
}

u8 Arm::executeThumbDataProcessingReg(ThumbInstruction& ins)
{
	u8 opcode = ins.opcode5();

	RegisterID rm = ins.rmLower();
	RegisterID rd = ins.rdLower();
	RegisterID rn = ins.rnLower();
	RegisterID rs = ins.rs();

	switch (opcode) {
		case 0b0000: thumbOpAND(ins, rm, rd); break;
		case 0b0001: thumbOpEOR(ins, rm, rd); break;
		case 0b0010: thumbOpLSL(ins, rs, rd); break;
		case 0b0011: thumbOpLSR(ins, rs, rd); break;
		case 0b0100: thumbOpASR(ins, rs, rd); break;
		case 0b0101: thumbOpADC(ins, rm, rd); break;
		case 0b0110: thumbOpSBC(ins, rm, rd); break;
		case 0b0111: thumbOpROR(ins, rs, rd); break;
		case 0b1000: thumbOpTST(ins, rm, rn); break;
		case 0b1001: thumbOpNEG(ins, rm, rd); break;
		case 0b1010: thumbOpCMP(ins, rm, rn, false); break;
		case 0b1011: thumbOpCMN(ins, rm, rn); break;
		case 0b1100: thumbOpORR(ins, rm, rd); break;
		case 0b1101: thumbOpMUL(ins, rm, rd); break;
		case 0b1110: thumbOpBIC(ins, rm, rd); break;
		case 0b1111: thumbOpMVN(ins, rm, rd); break;
	}

	return 1;
}

u8 Arm::executeThumbAddSubReg(ThumbInstruction& ins)
{
	u8 opc = ins.opc();
	RegisterID rm = ins.rmUpper();
	RegisterID rn = ins.rnMiddle();
	RegisterID rd = ins.rdLower();
	if (opc == 0x0) {
		thumbOpADD(ins, rm, rn, rd);
	}
	else {
		thumbOpSUB(ins, rm, rn, rd);
	}

	return 1;
}

u8 Arm::executeThumbAddSubImm(ThumbInstruction& ins)
{
	u8 opc = ins.opc();
	RegisterID rn = ins.rnMiddle();
	RegisterID rd = ins.rdLower();
	u8 imm3 = ins.imm3();

	if (opc == 0x0) {
		thumbOpADD(ins, rn, rd, imm3);
	}
	else {
		thumbOpSUB(ins, rn, rd, imm3);
	}

	return 1;
}

u8 Arm::executeThumbLoadStoreMultiple(ThumbInstruction& ins)
{
	u8 L = ins.L();
	if (L == 0x0) {
		thumbOpSTMIA(ins);
	}
	else {
		thumbOpLDMIA(ins);
	}

	return 1;
}

u8 Arm::executeThumbBranchExchange(ThumbInstruction& ins)
{
	u8 L = (ins.encoding >> 7) & 0x1;
	if (L == 0x0) {
		thumbOpBX(ins);
	}

	return 1;
}

u8 Arm::executeThumbSpecialDataProcessing(ThumbInstruction& ins)
{
	//Hi registers
	u8 opcode = ins.opcode();

	RegisterID rm = ins.rmLower();
	RegisterID rn = ins.rnLower();
	RegisterID rd = ins.rdLower();

	switch (opcode) {
		case 0b00: thumbOpADD(ins, rm, rd); break;
		case 0b01: thumbOpCMP(ins, rm, rn, true); break;
		case 0b10: thumbOpMOV(ins, rm, rd); break;
	}

	return 1;
}

u8 Arm::executeThumbLoadStoreStack(ThumbInstruction& ins)
{
	u8 L = ins.L();
	if (L == 0x0)
		thumbOpSTRStack(ins);
	else
		thumbOpLDRStack(ins);

	return 1;
}

u8 Arm::executeThumbAddSPOrPC(ThumbInstruction& ins)
{
	u8 reg = ins.reg();
	RegisterID rd = ins.rdUpper();
	if (reg == 0x0)
		thumbOpADD(ins, rd, true);
	else
		thumbOpADD(ins, rd, false);

	return 1;
}

u8 Arm::executeThumbMisc(ThumbInstruction& ins)
{
	bool add_sub_sp = (((ins.encoding >> 8) & 0xF) == 0b0000);
	bool push_pop_reg_list = (((ins.encoding >> 9) & 0x3) == 0b10);

	if (add_sub_sp) {
		u8 opc = ((ins.encoding) >> 7) & 0x1;
		//Misc Add/Sub SP
		if (opc == 0x0) {
			thumbOpADD(ins, ins.imm7());
		}else{
			thumbOpSUB(ins, ins.imm7());
		}
	}
	else if (push_pop_reg_list) {
		u8 L = ins.L();
		if (L == 0x1) {
			thumbOpPOP(ins);
		}
		else {
			thumbOpPUSH(ins);
		}
	}

	return 1;
}

u8 Arm::handleUndefinedThumbIns(ThumbInstruction& ins)
{
	u8 lutIndex = ins.instruction();
	printf("LUT Index: %d ", lutIndex);
	printf("THUMB undefined or unimplemented instruction 0x%04X at PC: 0x%08X\n", ins.encoding, R15 - 8);

	return 1;
}

u8 Arm::opMOV(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);
	
	
	u32 result = shifter_op;
	if (reg_rd == R15) {
		writeRegister(rd, result);
		flushPipeline();
	}
	else {
		writeRegister(rd, result);
	}
	
	if (flags) {
		setCC(result, rd, false, false, true, shifter_carry_out);
	}

	return 1;
}

u8 Arm::opADD(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	//Shifter carry out is not needed here
	u8 shifter_carry_out = 0;
	bool carry = false;
	bool overflow = false;

	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn + shifter_op;
	reg_rd = result;
	writeRegister(rd, reg_rd);

	carry = carryFrom(reg_rn, shifter_op);
	overflow = overflowFromAdd(reg_rn, shifter_op);
	
	if (flags) {
		setCC(result, rd, !carry, overflow);
	}

	return 1;
}

u8 Arm::opAND(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn & shifter_op;
	reg_rd = result;
	writeRegister(rd, reg_rd);

	if (flags) {
		setCC(result, rd, false, false, true, shifter_carry_out);
	}

	return 1;
}


u8 Arm::opEOR(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 1;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn ^ shifter_op;
	reg_rd = result;
	writeRegister(rd, reg_rd);

	if (flags) {
		setCC(result, rd, false, false, true, shifter_carry_out);
	}

	return 1;
}

u8 Arm::opSUB(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	bool borrow = false;
	bool overflow = false;

	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn - shifter_op;
	if (reg_rd != R15) {
		reg_rd = result;
		writeRegister(rd, reg_rd);

		borrow = borrowFrom(reg_rn, shifter_op);
		overflow = overflowFromSub(reg_rn, shifter_op);
	}
	else {
		R15 = result & 0xFFFFFFFC;
		flushPipeline();
	}
	
	if (flags) {
		setCC(result, rd, borrow, overflow);
	}

	return 1;
}

u8 Arm::opRSB(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	bool borrow = false;
	bool overflow = false;

 	u32 shifter_op = (immediate == true) ? 
		addrMode1.imm(ins, shifter_carry_out) :  addrMode1.shift(ins, shifter_carry_out);
	
	u32 result = shifter_op - reg_rn;
	reg_rd = result;
	writeRegister(rd, reg_rd);

	borrow = borrowFrom(shifter_op, reg_rn);
	overflow = overflowFromSub(shifter_op, reg_rn);

	if (flags) {
		setCC(result, rd, borrow, overflow);
	}

	return 1;
}

u8 Arm::opADC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	bool carry = false;
	bool overflow = false;

	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn + (shifter_op + getFlag(C));
	reg_rd = result;
	writeRegister(rd, reg_rd);

	carry = carryFrom(reg_rn, shifter_op + getFlag(C));
	overflow = overflowFromAdd(reg_rn, shifter_op + getFlag(C));

	if (flags) {
		setCC(result, rd, !carry, overflow);
	}

	return 1;
}

u8 Arm::opSBC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	bool borrow = false;
	bool overflow = false;

	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn - shifter_op - !getFlag(C);
	reg_rd = result;
	writeRegister(rd, reg_rd);

	borrow = borrowFrom(reg_rn - shifter_op, !getFlag(C));
	overflow = overflowFromSub(reg_rn - shifter_op, !getFlag(C));

	if (flags) {
		setCC(result, rd, borrow, overflow);
	}

	return 1;
}

u8 Arm::opRSC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = shifter_op - reg_rn - !getFlag(C);
	reg_rd = result;
	writeRegister(rd, reg_rd);

	bool borrow = borrowFrom(shifter_op - reg_rn, !getFlag(C));
	bool overflow = overflowFromSub(shifter_op - reg_rn, !getFlag(C));

	if (flags) {
		setCC(result, rd, borrow, overflow);
	}

	return 1;
}

u8 Arm::opTST(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn & shifter_op;

	setCC(result, rd, false, false, true, shifter_carry_out);

	return 1;
}

u8 Arm::opTEQ(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn ^ shifter_op;

	setCC(result, rd, false, false, true, shifter_carry_out);

	return 1;
}

u8 Arm::opCMP(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn - shifter_op;

	bool borrow = borrowFrom(reg_rn, shifter_op);
	bool overflow = overflowFromSub(reg_rn, shifter_op);

	if (rd.id == R15_ID) {
		//Update regardless
		(result >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
		(result == 0) ? setFlag(Z) : clearFlag(Z);
		(borrow == false) ? setFlag(C) : clearFlag(C);
		(overflow == true) ? setFlag(V) : clearFlag(V);
	}

	setCC(result, rd, borrow, overflow);

	return 1;
}

u8 Arm::opCMN(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn + shifter_op;

	bool carry = carryFrom(reg_rn, shifter_op);
	bool overflow = overflowFromAdd(reg_rn, shifter_op);

	setCC(result, rd, !carry, overflow);

	return 1;
}

u8 Arm::opORR(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn | shifter_op;
	reg_rd = result;
	writeRegister(rd, reg_rd);

	if (flags) {
		setCC(result, rd, false, false, true, shifter_carry_out);
	}

	return 1;
}

u8 Arm::opBIC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn & ~(shifter_op);
	writeRegister(rd, result);

	if (flags) {
		setCC(result, rd, false, false, true, shifter_carry_out);
	}

	return 1;
}

u8 Arm::opMVN(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	reg_rd = ~(shifter_op);
	writeRegister(rd, reg_rd);

	if (flags) {
		setCC(reg_rd, rd, false, false, true, shifter_carry_out);
	}

	return 1;
}

u8 Arm::opB(ArmInstruction& ins)
{
	s32 offset = ins.offset();
	offset = signExtend32(offset, 24);
	//Aligns r15 for arm instruction reading
	offset <<= 2;

	R15 += offset;
	flushPipeline();

	return 1;
}

u8 Arm::opBL(ArmInstruction& ins)
{
	LR = R15 - 4;
	
	s32 offset = ins.offset();
	offset = signExtend32(offset, 24);
	offset <<= 2;

	R15 += offset;
	flushPipeline();

	return 1;
}

u8 Arm::opBX(ArmInstruction& ins)
{
	RegisterID rm = ins.rm();
	u32 reg_rm = getRegister(rm);
	
	(reg_rm & 0x1) == 1 ? setFlag(T) : clearFlag(T);
	
	if (getFlag(T) == 0x0) {
		R15 = reg_rm & 0xFFFFFFFC;
		flushPipeline();
	}
	else {
		R15 = reg_rm & 0xFFFFFFFE;
		flushThumbPipeline();
		R15 -= 2;
	}

	return 1;
}

u8 Arm::opSWI(ArmInstruction& ins)
{
	u8 swi_number = (ins.encoding >> 16) & 0xFF;
	if (swi_number == 0x6) {
		//printf("Failed test: 0x%08X\n", registers[0].value);

		//HLE Div
		s32 r0 = getRegister(RegisterID{ (u8)0 });
		s32 r1 = getRegister(RegisterID{ (u8)1 });

		s32 div_res = r0 / r1;
		writeRegister(RegisterID{ (u8)0 }, div_res);

		s32 mod_res = r0 % r1;
		writeRegister(RegisterID{ (u8)1 }, mod_res);
		
		u32 abs_res = abs(r0 / r1);
		writeRegister(RegisterID{ (u8)3 }, abs_res);
	}
	
	printf("ARM mode SWI at address: 0x%08X", R15 - 8);
	/*LR_svc = R15 - 4;
	SPSR_svc = CPSR;

	enterSupervisorMode();
	clearFlag(T);
	setFlag(I);

	R15 = 0x8;
	flushPipeline();*/

	return 1;
}


u8 Arm::opSTRH(ArmInstruction& ins, RegisterID rd, u32 address)
{
	u8 aligned = (address & 0x1);
	//Misaligned
	if (aligned != 0b0) {
		address &= 0xFFFFFFFE;
		u32 reg_rd = getRegister(rd);
		writeU16(address, reg_rd & 0xFFFF);
	}
	else {
		u32 reg_rd = getRegister(rd);
		writeU16(address, reg_rd & 0xFFFF);
	}

	return 1;
}


u8 Arm::opLDRH(ArmInstruction& ins, RegisterID rd, u32 address)
{
	u8 aligned = (address & 0x1);
	if (aligned != 0b0) {
		//force align address
		address &= 0xFFFFFFFE;

		//read value and ror
		u32 value = mbus->readU16(address);
		value = ror(value, 8 * aligned);

		writeRegister(rd, value);
	}
	else {
		u16 value = mbus->readU16(address);
		writeRegister(rd, value);
	}

	return 1;
}

u8 Arm::opLDRSB(ArmInstruction& ins, RegisterID rd, u32 address)
{
	u32 value = (s8)mbus->readU8(address);
	value = signExtend32(value, 8);
	writeRegister(rd, value);

	return 1;
}

u8 Arm::opLDRSH(ArmInstruction& ins, RegisterID rd, u32 address)
{
	u8 aligned = (address & 0x1);
	if (aligned != 0b0) {
		//LDRSB happens when misaligned LDRSH
		//read a signed byte from the misaligned address then sign extends it and loads it
		u32 value = (s8)mbus->readU8(address);
		value = signExtend32(value, 8);

		writeRegister(rd, value);
	}
	else {
		u32 value = (s16)mbus->readU16(address);
		value = signExtend32(value, 16);
		writeRegister(rd, value);
	}

	return 1;
}

u8 Arm::opSWP(ArmInstruction& ins, RegisterID rd, RegisterID rn)
{
	RegisterID rm = ins.rm();
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);

	u8 aligned = (reg_rn & 0x3);
	u32 temp = 0;
	if (aligned == 0b00) {
		temp = mbus->readU32(reg_rn);
	}
	//Misaligned
	else {
		reg_rn &= 0xFFFFFFFC;
		temp = mbus->readU32(reg_rn);
		temp = ror(temp, 8 * aligned);
	}
	mbus->writeU32(reg_rn, reg_rm);
	writeRegister(rd, temp);

	return 1;
}

u8 Arm::opSWPB(ArmInstruction& ins, RegisterID rd, RegisterID rn)
{
	RegisterID rm = ins.rm();
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);

	u32 temp = mbus->readU8(reg_rn);
	mbus->writeU8(reg_rn, reg_rm & 0xFF);
	writeRegister(rd, temp);

	return 1;
}

u8 Arm::opLDRB(ArmInstruction& ins, RegisterID rd, u32 address)
{
	u8 value = mbus->readU8(address);
	writeRegister(rd, value);

	return 1;
}

u8 Arm::opLDR(ArmInstruction& ins, RegisterID rd, u32 address)
{
	u8 aligned = (address & 0x3);

	bool r15 = (rd.id == R15_ID);
	//Misaligned
	if (aligned != 0b00) {
		//force align address
		address &= 0xFFFFFFFC;

		u32 value = mbus->readU32(address);
		value = ror(value, 8 * aligned);

		if (r15) {
			writeRegister(rd, value);
			flushPipeline();
		}
		else
			writeRegister(rd, value);
	}
	//Aligned
	else {
		u32 value = mbus->readU32(address);

		if (r15) {
			writeRegister(rd, value);
			flushPipeline();
		}
		else
			writeRegister(rd, value);
	}

	return 1;
}

u8 Arm::opSTRB(ArmInstruction& ins, RegisterID rd, u32 address)
{
	u32 reg_rd = getRegister(rd);
	mbus->writeU8(address, reg_rd & 0xFF);

	return 1;
}

u8 Arm::opSTR(ArmInstruction& ins, RegisterID rd, u32 address)
{
	//force align address
	address &= 0xFFFFFFFC;

	u32 reg_rd = getRegister(rd);
	//R15 edge case
	if (rd.id == R15_ID) reg_rd += 4;

	mbus->writeU32(address, reg_rd);

	return 1;
}

u8 Arm::opMRS(ArmInstruction& ins)
{
	RegisterID rd = ins.rd();
	u8 R = ins.R();

	if (R == 0x1) {
		u32 spsr = getSPSR();
		writeRegister(rd, spsr);
	}
	else {
		writeRegister(rd, CPSR);
	}

	return 1;
}

u8 Arm::opMSR(ArmInstruction& ins, u32 value)
{
	u8 R = ins.R();
	u8 fm = ins.fieldMask();
	bool cpsr_write = (R == 0x0);

	if (cpsr_write) {
		if (((fm & 0x1) == 0x1) && inPrivilegedMode()) {
			u32 operand = value & 0xFF;
			for (u32 i = 0; i <= 7; i++)
				CPSR = resetBit(CPSR, i);

			CPSR |= operand;
		}
		if ((((fm >> 3) & 0x1) == 0x1)) {
			u32 operand = (value >> V_BIT) & 0xF;
			for (u32 i = 28; i <= 31; i++)
				CPSR = resetBit(CPSR, i);

			CPSR |= (operand << V_BIT);
		}
	}
	else { //spsr write
		if (((fm & 0x1) == 0x1) && currentModeHasSPSR()) {
			u32 spsr = getSPSR();
			u32 operand = value & 0xFF;
			for (u32 i = 0; i <= 7; i++)
				spsr = resetBit(spsr, i);

			spsr |= operand;

			writeSPSR(spsr);
		}
		if ((((fm >> 3) & 0x1) == 0x1) && currentModeHasSPSR()) {
			u32 spsr = getSPSR();
			u32 operand = (value >> V_BIT) & 0xF;
			for (u32 i = 28; i <= 31; i++)
				spsr = resetBit(spsr, i);

			spsr |= (operand << V_BIT);

			writeSPSR(spsr);
		}
	}

	return 1;
}

u8 Arm::opMUL(ArmInstruction& ins, RegisterID rd, RegisterID rm, RegisterID rs, bool flags)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rm = getRegister(rm);
	u32 reg_rs = getRegister(rs);

	u32 result = (reg_rm * reg_rs);
	reg_rd = result;
	writeRegister(rd, reg_rd);

	if (flags) {
		(result >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
		(result == 0) ? setFlag(Z) : clearFlag(Z);
	}

	return 1;
}

u8 Arm::opMLA(ArmInstruction& ins, RegisterID rd, RegisterID rm, RegisterID rs, bool flags)
{
	u8 rn_id = (ins.encoding >> 12) & 0xF;
	u32 reg_rn = getRegister(RegisterID{ (u8)rn_id });

	u32 reg_rd = getRegister(rd);
	u32 reg_rm = getRegister(rm);
	u32 reg_rs = getRegister(rs);

	u32 result = (reg_rm * reg_rs + reg_rn);
	reg_rd = result;
	writeRegister(rd, result);

	if (flags) {
		(result >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
		(result == 0) ? setFlag(Z) : clearFlag(Z);
	}

	return 1;
}

u8 Arm::opSMLAL(ArmInstruction& ins, RegisterID rdhi, RegisterID rdlo, RegisterID rm, RegisterID rs, bool flags)
{
	u32 reg_rdhi = getRegister(rdhi);
	u32 reg_rdlo = getRegister(rdlo);

	s32 reg_rm = getRegister(rm);
	s32 reg_rs = getRegister(rs);

	s64 reg_rd = ((s64)reg_rdhi << 32) | ((s64)reg_rdlo);
	s64 result = ((s64)reg_rm * (s64)reg_rs) + reg_rd;

	writeRegister(rdlo, result & 0xFFFFFFFF);
	writeRegister(rdhi, (result >> 32));

	if (flags) {
		(result >> 63) & 0x1 ? setFlag(N) : clearFlag(N);
		(result == 0) ? setFlag(Z) : clearFlag(Z);
	}

	return 1;
}

u8 Arm::opSMULL(ArmInstruction& ins, RegisterID rdhi, RegisterID rdlo, RegisterID rm, RegisterID rs, bool flags)
{
	s32 reg_rdhi = getRegister(rdhi);
	s32 reg_rdlo = getRegister(rdlo);

	s32 reg_rm = getRegister(rm);
	s32 reg_rs = getRegister(rs);

	s64 result = ((s64)reg_rm * (s64)reg_rs);
	reg_rdhi = (result >> 32);
	reg_rdlo = (result & 0xFFFFFFFF);

	writeRegister(rdhi, reg_rdhi);
	writeRegister(rdlo, reg_rdlo);

	if (flags) {
		(reg_rdhi >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
		((reg_rdhi == 0) && (reg_rdlo == 0)) ? setFlag(Z) : clearFlag(Z);
	}

	return 1;
}

u8 Arm::opUMLAL(ArmInstruction& ins, RegisterID rdhi, RegisterID rdlo, RegisterID rm, RegisterID rs, bool flags)
{
	u32 reg_rdhi = getRegister(rdhi);
	u32 reg_rdlo = getRegister(rdlo);

	u32 reg_rm = getRegister(rm);
	u32 reg_rs = getRegister(rs);

	u64 reg_rd = ((u64)reg_rdhi << 32) | ((u64)reg_rdlo);
	u64 result = ((u64)reg_rm * (u64)reg_rs) + reg_rd;

	writeRegister(rdlo, result & 0xFFFFFFFF);
	writeRegister(rdhi, (result >> 32));

	if (flags) {
		(result >> 63) & 0x1 ? setFlag(N) : clearFlag(N);
		(result == 0) ? setFlag(Z) : clearFlag(Z);
	}

	return 1;
}

u8 Arm::opUMULL(ArmInstruction& ins, RegisterID rdhi, RegisterID rdlo, RegisterID rm, RegisterID rs, bool flags)
{
	u32 reg_rdhi = getRegister(rdhi);
	u32 reg_rdlo = getRegister(rdlo);

	u32 reg_rm = getRegister(rm);
	u32 reg_rs = getRegister(rs);

	u64 result = (u64)reg_rm * (u64)reg_rs;
	reg_rdhi = (result >> 32);
	reg_rdlo = (result & 0xFFFFFFFF);

	writeRegister(rdhi, reg_rdhi);
	writeRegister(rdlo, reg_rdlo);

	if (flags) {
		(reg_rdhi >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
		((reg_rdhi == 0) && (reg_rdlo == 0)) ? setFlag(Z) : clearFlag(Z);
	}

	return 1;
}

//Thumb instructions

u8 Arm::thumbOpBCond(ThumbInstruction& ins)
{
	u8 cond = getConditionCode(ins.cond());
	if (cond) {
		s32 signed_imm8_offset = ins.signedImm8();
		signed_imm8_offset = signExtend32(signed_imm8_offset, 8);
		//Aligns r15 for thumb instruction reading
		signed_imm8_offset <<= 1;

		R15 += signed_imm8_offset;

		flushThumbPipeline();
	}

	return 1;
}

u8 Arm::thumbOpB(ThumbInstruction& ins)
{
	s32 signed_imm11_offset = ins.signedImm11();
	signed_imm11_offset = signExtend32(signed_imm11_offset, 11);
	signed_imm11_offset <<= 1;

	R15 += signed_imm11_offset;

	flushThumbPipeline();

	return 1;
}

u8 Arm::thumbOpBL(ThumbInstruction& ins)
{
	u8 H = ins.H();
	switch (H) {
		case 0b10:
		{
			s32 offset11 = ins.offset11();
			offset11 = signExtend32(offset11, 11);
			offset11 <<= 12;
	
			LR = R15 + offset11;
		}
		break;

		case 0b11:
		{
			s32 offset11 = (ins.offset11()) << 1;

			u32 prevLR = LR;
			LR = (R15 - 2) | 0x1;
			R15 = prevLR + offset11;

			flushThumbPipeline();
		}
		break;
	}
	
	return 1;
}

u8 Arm::thumbOpLDRPool(ThumbInstruction& ins)
{
	RegisterID rd = ins.rdUpper();
	u8 imm8 = ins.imm8();

	//lower 2 bits of PC are disregarded to word align
	u32 address = (R15 & 0xFFFFFFFC) + (imm8 * 4);
	u32 value = mbus->readU32(address);

	writeRegister(rd, value);

	return 1;
}

u8 Arm::thumbOpLDRStack(ThumbInstruction& ins)
{
	RegisterID rd = ins.rdUpper();
	u32 reg_rd = getRegister(rd);

	u32 sp = getRegister(RegisterID{ R13_ID });
	u8 imm8 = ins.imm8();

	u32 address = sp + (imm8 * 4);

	u8 aligned = (address & 0x1);
	if (aligned != 0b0) {
		//force align address
		address &= 0xFFFFFFFE;

		//read value and ror
		u32 value = mbus->readU32(address);
		value = ror(value, 8 * aligned);
		reg_rd = value;

		writeRegister(rd, reg_rd);
	}
	else {
		u32 value = mbus->readU32(address);
		reg_rd = value;
		writeRegister(rd, reg_rd);
	}

	return 1;
}

u8 Arm::thumbOpSTRStack(ThumbInstruction& ins)
{
	RegisterID rd = ins.rdUpper();
	u32 reg_rd = getRegister(rd);

	u32 sp = getRegister(RegisterID{ R13_ID });
	u8 imm8 = ins.imm8();

	u32 address = sp + (imm8 * 4);	
	mbus->writeU32(address, reg_rd);

	return 1;
}


u8 Arm::thumbOpASR(ThumbInstruction& ins, u8 immediate5)
{
	RegisterID rd = ins.rdLower();
	RegisterID rm = ins.rmLower();

	u32 reg_rd = getRegister(rd);
	u32 reg_rm = getRegister(rm);

	u8 shifter_carry_out = 0;
	//shift by 32
	if (immediate5 == 0) {
		u8 rm_sign_bit = (reg_rm >> 31) & 0x1;
		(rm_sign_bit == 1) ? setFlag(C) : clearFlag(C);
		
		if (rm_sign_bit == 0)
			reg_rd = 0x0;
		else
			reg_rd = 0xFFFFFFFF;
	}
	else { //imm > 0
		u8 shiftedBit = (immediate5 - 1);
		shiftedBit = (reg_rm >> shiftedBit) & 0x1;

		(shiftedBit == 1) ? setFlag(C) : clearFlag(C);

		u32 result = reg_rm >> immediate5;
		result = signExtend32(result, 32 - immediate5);

		reg_rd = result;
	}
	
	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	return 1;
}

u8 Arm::thumbOpASR(ThumbInstruction& ins, RegisterID rs, RegisterID rd)
{
	u32 reg_rs = getRegister(rs);
	u32 reg_rd = getRegister(rd);

	u8 shift_amount = reg_rs & 0xFF;
	//shift by 32
	if (shift_amount == 0x0) {
		//C flag unaffected
		//rd unaffected
	}
	else if (shift_amount < 32) {
		u8 shiftedBit = (shift_amount - 1);
		shiftedBit = (reg_rd >> shiftedBit) & 0x1;

		(shiftedBit == 1) ? setFlag(C) : clearFlag(C);

		u32 result = reg_rd >> shift_amount;
		result = signExtend32(result, 32 - shift_amount);

		reg_rd = result;
	}
	//shift_amount >= 32
	else {
		bool rd_sign_bit = (reg_rd >> 31) & 0x1;
		(rd_sign_bit == 1) ? setFlag(C) : clearFlag(C);

		if (rd_sign_bit == 0) {
			reg_rd = 0x0;
		}
		else {
			reg_rd = 0xFFFFFFFF;
		}
	}
	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	return 1;
}

u8 Arm::thumbOpLSL(ThumbInstruction& ins, u8 immediate5)
{
	RegisterID rd = ins.rdLower();
	RegisterID rm = ins.rmLower();

	u32 reg_rd = getRegister(rd);
	u32 reg_rm = getRegister(rm);

	//if zero, Simply load the register
	if (immediate5 == 0) {
		//C flag unaffected
		reg_rd = reg_rm;
	}
	//imm5 > 0
	else {
		u8 shifter_carry_out = 0;
		reg_rd = lsl(reg_rm, immediate5, shifter_carry_out, true);

		(shifter_carry_out == 1) ? setFlag(C) : clearFlag(C);
	}
	writeRegister(rd, reg_rd);
	
	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	return 1;
}

u8 Arm::thumbOpLSL(ThumbInstruction& ins, RegisterID rs, RegisterID rd)
{
	u32 reg_rs = getRegister(rs);
	u32 reg_rd = getRegister(rd);

	u8 shift_amount = reg_rs & 0xFF;
	if (shift_amount == 0) {
		//C flag unaffected
		//rd unaffected
	}
	else if (shift_amount < 32) {
		u8 shifter_carry_out = 0;
		reg_rd = lsl(reg_rd, shift_amount, shifter_carry_out, false);

		(shifter_carry_out == 1) ? setFlag(C) : clearFlag(C);
	}
	else if (shift_amount == 32) {
		(reg_rd & 0x1) ? setFlag(C) : clearFlag(C);
		reg_rd = 0x0;
	}
	//shift_amount > 32
	else {
		clearFlag(C);
		reg_rd = 0x0;
	}
	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	return 1;
}

u8 Arm::thumbOpLSR(ThumbInstruction& ins, u8 immediate5)
{
	RegisterID rm = ins.rmLower();
	RegisterID rd = ins.rdLower();

	u32 reg_rm = getRegister(rm);
	u32 reg_rd = getRegister(rd);

	//shift by 32
	if (immediate5 == 0) {
		(reg_rm >> 31) & 0x1 ? setFlag(C) : clearFlag(C);
		reg_rd = 0x0;
	}
	else {
		u8 shifter_carry_out = 0;
		reg_rd = lsr(reg_rm, immediate5, shifter_carry_out, true);

		(shifter_carry_out == 1) ? setFlag(C) : clearFlag(C);
	}

	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	return 1;
}

u8 Arm::thumbOpLSR(ThumbInstruction& ins, RegisterID rs, RegisterID rd)
{
	u32 reg_rs = getRegister(rs);
	u32 reg_rd = getRegister(rd);

	u8 shift_amount = reg_rs & 0xFF;

	u8 shifter_carry_out = 0;
	reg_rd = lsr(reg_rd, shift_amount, shifter_carry_out, false);

	(shifter_carry_out == 1) ? setFlag(C) : clearFlag(C);

	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	return 1;
}

u8 Arm::thumbOpMOV(ThumbInstruction& ins, RegisterID rm, RegisterID rd)
{	
	u8 h1 = ins.h1();
	u8 h2 = ins.h2();

	RegisterID actual_reg_rm;
	actual_reg_rm.id = (h2 << 3) | rm.id;

	RegisterID actual_reg_rd;
	actual_reg_rd.id = (h1 << 3) | rd.id;

	u32 reg_rd = getRegister(actual_reg_rd);
	u32 reg_rm = getRegister(actual_reg_rm);

	if (actual_reg_rd.id == R15_ID) {
		R15 = reg_rm & 0xFFFFFFFE;
		flushThumbPipeline();
	}
	else
		writeRegister(actual_reg_rd, reg_rm);

	return 1;
}

u8 Arm::thumbOpMOV(ThumbInstruction& ins, u8 immediate)
{
	RegisterID rd = ins.rdUpper();
	u32 reg_rd = getRegister(rd);

	reg_rd = immediate;
	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);
	
	return 1;
}

u8 Arm::thumbOpMUL(ThumbInstruction& ins, RegisterID rm, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rd = getRegister(rd);

	reg_rd = reg_rm * reg_rd;
	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	//clear carry because NBA clears carry for mul (unsure if hardware does the same)
	clearFlag(C);

	return 1;
}

u8 Arm::thumbOpNEG(ThumbInstruction& ins, RegisterID rm, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rd = getRegister(rd);

	reg_rd = 0 - reg_rm;
	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	bool borrow = borrowFrom(0, reg_rm);
	bool overflow = overflowFromSub(0, reg_rm);

	setCC(reg_rd, rd, borrow, overflow);

	return 1;
}

u8 Arm::thumbOpORR(ThumbInstruction& ins, RegisterID rm, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rd = getRegister(rd);

	reg_rd = reg_rd | reg_rm;
	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	return 1;
}

u8 Arm::thumbOpMVN(ThumbInstruction& ins, RegisterID rm, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rd = getRegister(rd);

	reg_rd = ~(reg_rm);
	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	return 1;
}

u8 Arm::thumbOpROR(ThumbInstruction& ins, RegisterID rs, RegisterID rd)
{
	u32 reg_rs = getRegister(rs);
	u32 reg_rd = getRegister(rd);

	u8 rotate_amount = reg_rs & 0xFF;
	u8 rot_lower = rotate_amount & 0x1F;
	if (rotate_amount == 0) {
		//C flag unaffected
		//rd unaffected
	}
	else if (rot_lower == 0) {
		(reg_rd >> 31) & 0x1 ? setFlag(C) : clearFlag(C);
		//rd unaffected
	}
	//rot_lower > 0
	else {
		u32 c = (rot_lower - 1);
		c = (reg_rd >> c) & 0x1;
		(c == 1) ? setFlag(C) : clearFlag(C);

		reg_rd = ror(reg_rd, rot_lower);
	}
	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	return 1;
}

u8 Arm::thumbOpADC(ThumbInstruction& ins, RegisterID rm, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rd = getRegister(rd);

	u32 result = (reg_rd + reg_rm) + getFlag(C);
	writeRegister(rd, result);

	bool carry = carryFrom(reg_rd, reg_rm + getFlag(C));
	bool overflow = overflowFromAdd(reg_rd, reg_rm + getFlag(C));

	setCC(result, rd, !carry, overflow);

	return 1;
}

u8 Arm::thumbOpADD(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	reg_rd = reg_rn + reg_rm;
	writeRegister(rd, reg_rd);

	bool carry = carryFrom(reg_rn, reg_rm);
	bool overflow = overflowFromAdd(reg_rn, reg_rm);

	setCC(reg_rd, rd, !carry, overflow);

	return 1;
}

u8 Arm::thumbOpADD(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate)
{
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	//it's a mov encoded as add rd, rn, #0
	if (immediate == 0) {
		reg_rd = reg_rn;
		writeRegister(rd, reg_rd);

		(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
		(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);
		clearFlag(C);
		clearFlag(V);
	}
	else {
		reg_rd = reg_rn + immediate;
		writeRegister(rd, reg_rd);

		bool carry = carryFrom(reg_rn, immediate);
		bool overflow = overflowFromAdd(reg_rn, immediate);

		setCC(reg_rd, rd, !carry, overflow);
	}

	return 1;
}

u8 Arm::thumbOpADD(ThumbInstruction& ins, RegisterID rd, u8 immediate)
{
	u32 reg_rd = getRegister(rd);

	u32 result = reg_rd + immediate;
	writeRegister(rd, result);

	bool carry = carryFrom(reg_rd, immediate);
	bool overflow = overflowFromAdd(reg_rd, immediate);

	setCC(result, rd, !carry, overflow);

	return 1;
}

u8 Arm::thumbOpADD(ThumbInstruction& ins, RegisterID rm, RegisterID rd)
{
	u8 h1 = ins.h1();
	u8 h2 = ins.h2();

	RegisterID actual_reg_rm;
	actual_reg_rm.id = (h2 << 3) | rm.id;

	RegisterID actual_reg_rd;
	actual_reg_rd.id = (h1 << 3) | rd.id;

	u32 reg_rm = getRegister(actual_reg_rm);
	u32 reg_rd = getRegister(actual_reg_rd);

	u32 result = reg_rd + reg_rm;

	if (actual_reg_rd.id == R15_ID) {
		R15 = result & 0xFFFFFFFC;
		flushThumbPipeline();
	}
	else
		writeRegister(actual_reg_rd, result);

	return 1;
}

u8 Arm::thumbOpADD(ThumbInstruction& ins, RegisterID rd, bool pc)
{
	u32 reg_rd = getRegister(rd);
	u8 imm8 = ins.imm8();
	//Add pc
	if (pc) {
		reg_rd = (R15 & 0xFFFFFFFC) + (imm8 * 4);
	}
	//add sp
	else {
		u32 sp = getRegister(RegisterID{ R13_ID });
		reg_rd = sp + (imm8 * 4);
	}

	writeRegister(rd, reg_rd);

	return 1;
}

u8 Arm::thumbOpADD(ThumbInstruction& ins, u8 immediate7)
{
	u32 sp = getRegister(RegisterID{ R13_ID });

	sp = sp + (immediate7 * 4);
	writeRegister(RegisterID{ R13_ID }, sp);

	return 1;
}

u8 Arm::thumbOpAND(ThumbInstruction& ins, RegisterID rm, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rd = getRegister(rd);

	reg_rd = reg_rd & reg_rm;
	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	return 1;
}

u8 Arm::thumbOpCMN(ThumbInstruction& ins, RegisterID rm, RegisterID rn)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);

	u32 result = reg_rn + reg_rm;

	(result >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(result == 0) ? setFlag(Z) : clearFlag(Z);

	bool carry = carryFrom(reg_rn, reg_rm);
	bool overflow = overflowFromAdd(reg_rn, reg_rm);

	//rd id as 0 when no destination reg
	setCC(result, RegisterID{ (u8)0 }, !carry, overflow);

	return 1;
}

u8 Arm::thumbOpCMP(ThumbInstruction& ins, RegisterID rm, RegisterID rn, bool hiRegisters)
{
	if (hiRegisters) {
		u8 h1 = ins.h1();
		u8 h2 = ins.h2();

		RegisterID actual_reg_rn_id;
		actual_reg_rn_id.id = (h1 << 3) | rn.id;

		RegisterID actual_reg_rm_id;
		actual_reg_rm_id.id = (h2 << 3) | rm.id;

		u32 reg_rn = getRegister(actual_reg_rn_id);
		u32 reg_rm = getRegister(actual_reg_rm_id);
		
		u32 result = reg_rn - reg_rm;

		bool borrow = borrowFrom(reg_rn, reg_rm);
		bool overflow = overflowFromSub(reg_rn, reg_rm);

		setCC(result, RegisterID{ (u8)0 }, borrow, overflow);
	}
	else {

		u32 reg_rm = getRegister(rm);
		u32 reg_rn = getRegister(rn);

		u32 result = reg_rn - reg_rm;

		bool borrow = borrowFrom(reg_rn, reg_rm);
		bool overflow = overflowFromSub(reg_rn, reg_rm);

		setCC(result, RegisterID{ (u8)0 }, borrow, overflow);
	}

	return 1;
}

u8 Arm::thumbOpCMP(ThumbInstruction& ins, u8 immediate)
{
	RegisterID rn = ins.rnUpper();
	u32 reg_rn = getRegister(rn);

	u32 result = reg_rn - immediate;

	bool borrow = borrowFrom(reg_rn, immediate);
	bool overflow = overflowFromSub(reg_rn, immediate);

	setCC(result, RegisterID{ (u8)0 }, borrow, overflow);

	return 1;
}

u8 Arm::thumbOpEOR(ThumbInstruction& ins, RegisterID rm, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rd = getRegister(rd);

	reg_rd = reg_rd ^ reg_rm;
	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	return 1;
}

u8 Arm::thumbOpSBC(ThumbInstruction& ins, RegisterID rm, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rd = getRegister(rd);

	u32 result = (reg_rd - reg_rm) - (!(getFlag(C)));
	writeRegister(rd, result);

	bool borrow = borrowFrom(reg_rd, reg_rm - (!(getFlag(C))));
	bool overflow = overflowFromSub(reg_rd, reg_rm - (!(getFlag(C))));

	setCC(result, rd, borrow, overflow);

	return 1;
}

u8 Arm::thumbOpSUB(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	reg_rd = reg_rn - reg_rm;
	writeRegister(rd, reg_rd);

	bool borrow = borrowFrom(reg_rn, reg_rm);
	bool overflow = overflowFromSub(reg_rn, reg_rm);

	setCC(reg_rd, rd, borrow, overflow);

	return 1;
}

u8 Arm::thumbOpSUB(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate)
{
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	reg_rd = reg_rn - immediate;
	writeRegister(rd, reg_rd);

	bool borrow = borrowFrom(reg_rn, immediate);
	bool overflow = overflowFromSub(reg_rn, immediate);

	setCC(reg_rd, rd, borrow, overflow);

	return 1;
}

u8 Arm::thumbOpSUB(ThumbInstruction& ins, RegisterID rd, u8 immediate)
{
	u32 reg_rd = getRegister(rd);

	u32 result = reg_rd - immediate;
	writeRegister(rd, result);

	bool borrow = borrowFrom(reg_rd, immediate);
	bool overflow = overflowFromSub(reg_rd, immediate);

	setCC(result, rd, borrow, overflow);

	return 1;
}

u8 Arm::thumbOpSUB(ThumbInstruction& ins, u8 immediate7)
{
	u32 sp = getRegister(RegisterID{ R13_ID });

	sp = sp - (immediate7 * 4);
	writeRegister(RegisterID{ R13_ID }, sp);

	return 1;
}

u8 Arm::thumbOpBIC(ThumbInstruction& ins, RegisterID rm, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rd = getRegister(rd);

	reg_rd = reg_rd & ~(reg_rm);
	writeRegister(rd, reg_rd);

	(reg_rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);

	return 1;
}

u8 Arm::thumbOpTST(ThumbInstruction& ins, RegisterID rm, RegisterID rn)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);

	u32 result = reg_rn & reg_rm;

	(result >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(result == 0) ? setFlag(Z) : clearFlag(Z);

	return 1;
}

u8 Arm::thumbOpSTMIA(ThumbInstruction& ins)
{
	RegisterID rn = ins.rnUpper();
	u32 reg_rn = getRegister(rn);

	u8 reg_list = ins.registerList();

	u32 reg_base = reg_rn;
	u32 address = reg_rn;
	address &= 0xFFFFFFFC;

	u32 end_address = reg_rn + (numSetBitsU8(reg_list) * 4) - 4;

	//Empty list
	if (reg_list == 0x0) {
		mbus->writeU32(address, R15 + 2);
		reg_rn += 0x40;
	}

	//Check if first reg in list is rb
	s8 first_reg = -1;
	for (s32 i = 0; i <= 7; i++) {
		if (testBit(reg_list, i)) {
			first_reg = i;
			break;
		}
	}

	//Is rb the first register in the list?
	bool first = false;
	if (rn.id == first_reg) {
		first = true;
	}

	if (first == false) {
		reg_rn += (numSetBitsU8(reg_list) * 4);
		writeRegister(rn, reg_rn);
	}

	for (s32 i = 0; i <= 7; i++) {
		bool included = testBit(reg_list, i);
		if (included) {
			RegisterID id; id.id = i;
			
			//If first, write the rb value to mem
			u32 reg = getRegister(id);
			mbus->writeU32(address, reg);
			address += 4;

			//then increment and writeback
			if (first) {
				reg_rn += (numSetBitsU8(reg_list) * 4);
				writeRegister(rn, reg_rn);
				first = false;
			}
		}

		if (end_address == address - 4) break;
	}

	return 1;
}

u8 Arm::thumbOpLDMIA(ThumbInstruction& ins)
{
	RegisterID rn = ins.rnUpper();
	u32 reg_rn = getRegister(rn);

	u8 reg_list = ins.registerList();

	u32 address = reg_rn;
	address &= 0xFFFFFFFC;

	u32 end_address = reg_rn + (numSetBitsU8(reg_list) * 4) - 4;

	//Empty list
	if (reg_list == 0x0) {
		//Load value from reg_rn address into r15
		u32 addr = mbus->readU32(address);
		R15 = addr & 0xFFFFFFFE;
		flushThumbPipeline();

		reg_rn += 0x40;
	}


	reg_rn += (numSetBitsU8(reg_list) * 4);
	writeRegister(rn, reg_rn);

	for (s32 i = 0; i <= 7; i++) {
		bool included = testBit(reg_list, i);
		if (included) {
			RegisterID id; id.id = i;
			
			u32 value = mbus->readU32(address);
			writeRegister(id, value);
			address += 4;
		}

		if (end_address == address - 4) break;
	}

	return 1;
}

u8 Arm::thumbOpBX(ThumbInstruction& ins)
{
	RegisterID rm = ins.rmLower();
	u8 h2 = ins.h2();

	RegisterID actual_reg_id;
	actual_reg_id.id = ((h2 << 3) | rm.id) & 0xF;

	u32 reg_rm = getRegister(actual_reg_id);

	(reg_rm & 0x1) == 1 ? setFlag(T) : clearFlag(T);
	
	if (getFlag(T) == 0x0) {
		R15 = reg_rm & 0xFFFFFFFC;
		flushPipeline();
		R15 += 2;
	}
	else {
		R15 = reg_rm & 0xFFFFFFFE;
		flushThumbPipeline();
	}

	return 1;
}

u8 Arm::thumbOpSWI(ThumbInstruction& ins)
{
	LR_svc = R15 - 2; //store address of next instruction after this one
	SPSR_svc = CPSR;

	enterSupervisorMode();
	clearFlag(T); //Execute in arm state
	setFlag(I); //disable normal interrupts

	R15 = 0x8; //jump to bios
	flushPipeline();

	return 1;
}

u8 Arm::thumbOpLDR(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate5)
{
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + (immediate5 * 4);

	u8 aligned = (address & 0x3);
	if (aligned != 0b00) {
		//force align address
		address &= 0xFFFFFFFC;

		//read value and ror
		u32 value = mbus->readU32(address);
		value = ror(value, 8 * aligned);
		reg_rd = value;

		writeRegister(rd, reg_rd);
	}
	else {
		u32 value = mbus->readU32(address);
		reg_rd = value;
		writeRegister(rd, reg_rd);
	}

	return 1;
}


u8 Arm::thumbOpLDR(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + reg_rm;

	//Misaligned
	u8 aligned = (address & 0x3);
	if (aligned != 0b00) {
		//force align address
		address &= 0xFFFFFFFC;

		//read value and ror
		u32 value = mbus->readU32(address);
		value = ror(value, 8 * aligned);
		reg_rd = value;

		writeRegister(rd, reg_rd);
	}
	//Aligned
	else {
		u32 value = mbus->readU32(address);
		reg_rd = value;
		writeRegister(rd, reg_rd);
	}

	return 1;
}

u8 Arm::thumbOpLDRB(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate5)
{
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + immediate5;
	u8 value = mbus->readU8(address);
	reg_rd = value;

	writeRegister(rd, reg_rd);

	return 1;
}

u8 Arm::thumbOpLDRB(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + reg_rm;
	u8 value = mbus->readU8(address);
	reg_rd = value;

	writeRegister(rd, reg_rd);

	return 1;
}

u8 Arm::thumbOpLDRSB(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + reg_rm;

	u32 value = (s8)mbus->readU8(address);
	value = signExtend32(value, 8);
	reg_rd = value;

	writeRegister(rd, reg_rd);

	return 1;
}

u8 Arm::thumbOpLDRSH(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + reg_rm;

	u8 aligned = (address & 0x1);
	if (aligned != 0b0) {
		//LDRSB happens when misaligned LDRSH
		//read a signed byte from the misaligned address then sign extend it and load it
		u32 value = (s8)mbus->readU8(address);
		value = signExtend32(value, 8);
		reg_rd = value;

		writeRegister(rd, reg_rd);
	}
	else {
		u32 value = (s16)mbus->readU16(address);
		value = signExtend32(value, 16);
		reg_rd = value;

		writeRegister(rd, reg_rd);
	}

	return 1;
}

u8 Arm::thumbOpLDRH(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate5)
{
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + (immediate5 * 2);

	u8 aligned = (address & 0x1);
	if (aligned != 0b0) {
		//force align address
		address &= 0xFFFFFFFE;

		//read value and ror
		u32 value = mbus->readU16(address);
		value = ror(value, 8 * aligned);
		reg_rd = value;

		writeRegister(rd, reg_rd);
	}
	else {
		u16 value = mbus->readU16(address);
		reg_rd = value;
		writeRegister(rd, reg_rd);
	}

	return 1;
}

u8 Arm::thumbOpLDRH(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + reg_rm;

	//Misaligned
	u8 aligned = (address & 0x1);
	if (aligned != 0b0) {
		//force align address
		address &= 0xFFFFFFFE;

		//read value and ror
		u32 value = mbus->readU16(address);
		value = ror(value, 8 * aligned);
		reg_rd = value;

		writeRegister(rd, reg_rd);
	}
	//Aligned
	else {
		u16 value = mbus->readU16(address);
		reg_rd = value;
		writeRegister(rd, reg_rd);
	}

	return 1;
}

u8 Arm::thumbOpSTR(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate5)
{
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + (immediate5 * 4);
	mbus->writeU32(address, reg_rd);

	return 1;
}

u8 Arm::thumbOpSTR(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + reg_rm;
	mbus->writeU32(address, reg_rd);

	return 1;
}

u8 Arm::thumbOpSTRB(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate5)
{
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + immediate5;
	mbus->writeU8(address, reg_rd & 0xFF);

	return 1;
}

u8 Arm::thumbOpSTRB(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + reg_rm;
	mbus->writeU8(address, reg_rd & 0xFF);

	return 1;
}

u8 Arm::thumbOpSTRH(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate5)
{
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + (immediate5 * 2);
	writeU16(address, reg_rd & 0xFFFF);

	return 1;
}

u8 Arm::thumbOpSTRH(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd)
{
	u32 reg_rm = getRegister(rm);
	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	u32 address = reg_rn + reg_rm;
	writeU16(address, reg_rd & 0xFFFF);

	return 1;
}

u8 Arm::thumbOpPUSH(ThumbInstruction& ins)
{
	u8 reg_list = ins.registerList();
	u32 sp = getRegister(RegisterID{ R13_ID });
	u8 R = ins.R(); 

	u32 start_address = sp - 4 * (R + numSetBitsU8(reg_list));
	start_address &= 0xFFFFFFFE;

	u32 end_address = sp - 4;
	u32 address = start_address;

	for (s32 i = 0; i <= 7; i++) {
		bool included = testBit(reg_list, i);
		if (included) {
			RegisterID id;
			id.id = i;

			u32 reg = getRegister(id);
			mbus->writeU32(address, reg);
			address += 4;
		}
	}

	//is LR included
	if (R == 0x1) {
		u32 lr = getRegister(RegisterID{ R14_ID });
		mbus->writeU32(address, lr);
	}
	sp = sp - 4 * (R + numSetBitsU8(reg_list));
	writeRegister(RegisterID{ R13_ID }, sp);

	return 1;
}

u8 Arm::thumbOpPOP(ThumbInstruction& ins)
{
	u8 reg_list = ins.registerList();
	u32 sp = getRegister(RegisterID{ R13_ID });
	u8 R = ins.R();

	u32 start_address = sp;
	start_address &= 0xFFFFFFFE;

	u32 end_address = sp + 4 * (R + numSetBitsU8(reg_list));
	u32 address = start_address;

	for (s32 i = 0; i <= 7; i++) {
		bool included = testBit(reg_list, i);
		if (included) {
			RegisterID id;
			id.id = i;

			u32 value = mbus->readU32(address);
			writeRegister(id, value);
			address += 4;
		}
	}

	if (R == 0x1) {
		u32 value = mbus->readU32(address);
		R15 = value & 0xFFFFFFFE;
		flushThumbPipeline();
	}
	sp = end_address;
	writeRegister(RegisterID{ R13_ID }, sp);

	return 1;
}



