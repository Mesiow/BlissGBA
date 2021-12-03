#pragma once
#include "Instruction.h"
#include "AddressingModes.h"
#include "Interrupts.h"
#include <array>
#include <functional>

using namespace std::placeholders;
#define b(x) std::bind(x, this, _1)

/*
	The ARM7TDMI is a 32bit RISC (Reduced Instruction Set Computer) CPU, 
	designed by ARM (Advanced RISC Machines)

	*uses the armv4 ISA
*/

//Current Program Status Register (CPSR) bits
//Mode bits M0 - M4
#define M0 (1 << 0)
#define M0_BIT 0
#define M1 (1 << 1)
#define M1_BIT 1
#define M2 (1 << 2)
#define M2_BIT 2
#define M3 (1 << 3)
#define M3_BIT 3
#define M4 (1 << 4)
#define M4_BIT 4

//State bit (0 = ARM, 1 = THUMB) 
//Do not change manually
#define T (1 << 5)
#define T_BIT 5

//FIQ disable (Fast interrupt request) (0 = enable, 1 = disable)
#define F (1 << 6)
#define F_BIT 6

//IRQ disable (0 = enable, 1 = disable)
#define I (1 << 7)
#define I_BIT 7

//Overflow flag (0 = no overflow, 1 = overflow)
#define V (1 << 28)
#define V_BIT 28

//Carry flag (0 = Borrow/No Carry, 1 = Carry/No Borrow)
#define C (1 << 29)
#define C_BIT 29

//Zero flag (0 = Not Zero, 1 = Zero)
#define Z (1 << 30)
#define Z_BIT 30

//Sign flag (0 = Not signed, 1 = Signed)
#define N (1 << 31)
#define N_BIT 31

//Control bits mask (0 - 7)
#define CONTROL_BITS (M0 | M1 | M2 | M3 | M4 | T | F | I)

//Flags field bit mask
#define FLAG_FIELD_BITS (V | C | Z | N)

//Program counter register format (R15)

#define NUM_REGISTERS 0xD
#define R13_ID 0xD
#define R14_ID 0xE
#define R15_ID 0xF
#define NUM_REGISTERS_FIQ 4

union Register {
	struct {
		u16 lo;
		u16 hi;
	};
	u32 value;
};

enum class State : u8 {
	ARM = 0,
	THUMB
};

enum class ProcessorMode : u8 {
	USER = 0, //Normal program execution
	FIQ, //High speed data transfer or channel process
	IRQ, //General purpose interrupt handling
	SVC, //Supervisor (protected mode for the OS)
	ABT, //Abort (Implements virtual memory/and or mem protection)
	UND, //Undefined (supports software emulation of hardware co-processors)
	SYS //System (privileged)
};

struct PSR {
	u32 CPSR;
	u32 SPSR;
};

class MemoryBus;

class Arm {
public:
	Arm(MemoryBus *mbus);
	u8 clock();
	void handleTimers();
	void handleInterrupts();
	void checkStateAndProcessorMode();
	void reset();
	void setFlag(u32 flagBits, bool condition);
	void setFlag(u32 flagBits);
	void clearFlag(u32 flagBits);
	void setFlagSPSR(u32 flagBits);
	void clearFlagSPSR(u32 flagBits);

	void fillPipeline();
	void flushPipeline();
	void flushThumbPipeline();

	void enterSupervisorMode();
	void enterIRQMode();

	u8 getFlag(u32 flag);
	u8 carryFrom(u32 op1, u32 op2);
	u8 borrowFrom(u32 op1, u32 op2);
	u8 overflowFromAdd(u32 op1, u32 op2);
	u8 overflowFromSub(u32 op1, u32 op2);
	u8 fetchOp(u32 encoding);
	PSR getPSR();
	u32 getSPSR();

	u32 getRegister(RegisterID reg);
	u32 getUserModeRegister(RegisterID reg);
	void writeRegister(RegisterID reg, u32 value);
	void writeUserModeRegister(RegisterID reg, u32 value);
	void writeSPSR(u32 spsr);
	void writePC(u32 pc);

	//Set condition codes (S bit)
	void setCC(u32 result, RegisterID rd, bool borrow, bool overflow,
		 bool shiftOut = false, u8 shifterCarryOut = 0);

	bool inPrivilegedMode();
	bool currentModeHasSPSR();
	ProcessorMode getProcessorMode();
	State getState();
	u8 getConditionCode(u8 cond);

	void writeU16(u32 address, u16 value);
	u16 readU16();
	u16 fetchU16();
	u32 readU32();
	u32 fetchU32();

	//If immediate == false, the value passed in is a register
	u32 shift(u32 value, u8 amount, u8 type, u8 &shiftedBit, bool immediate);
	u32 lsl(u32 value, u8 shift, u8& shiftedBit);
	u32 lsr(u32 value, u8 shift, u8& shiftedBit, bool immediate);
	u32 asr(u32 value, u8 shift, u8& shiftedBit, bool immediate);
	u32 ror(u32 value, u8 shift, u8 &shiftedBit, bool immediate);
	u32 ror(u32 value, u8 shift);
	u32 rrx(u32 value, u8 &shiftedBit);

	void executeArmIns(ArmInstruction& ins);
	void executeThumbIns(ThumbInstruction& ins);

	//Arm
	u8 executeDataProcessing(ArmInstruction& ins, bool flags, bool immediate);
	u8 executeDataProcessingImmFlags(ArmInstruction& ins);
	u8 executeDataProcessingImm(ArmInstruction& ins);
	u8 executeDataProcessingImmShiftFlags(ArmInstruction& ins);
	u8 executeDataProcessingImmShift(ArmInstruction& ins);
	u8 executeDataProcessingRegShiftFlags(ArmInstruction& ins);
	u8 executeDataProcessingRegShift(ArmInstruction& ins);
	u8 handleUndefinedIns(ArmInstruction& ins);

	u8 executeLoadStore(ArmInstruction& ins, AddrModeLoadStoreResult& result);
	u8 executeLoadStoreImm(ArmInstruction& ins);
	u8 executeLoadStoreShift(ArmInstruction& ins);

	u8 executeMiscLoadAndStore(ArmInstruction& ins, AddrModeLoadStoreResult &result);
	u8 executeMiscLoadStoreImm(ArmInstruction& ins);
	u8 executeMiscLoadStoreReg(ArmInstruction& ins);

	//Load/Store Multiple
	u8 executeLDM(ArmInstruction& ins);
	u8 executeSTM(ArmInstruction& ins);

	//Status Register
	u8 executeMSRImm(ArmInstruction& ins);
	u8 executeMSRReg(ArmInstruction& ins);

	u8 executeSwap(ArmInstruction& ins);

	//Multiply
	u8 executeMultiplyLong(ArmInstruction& ins);
	u8 executeMultiply(ArmInstruction& ins);
	

	//Thumb
	u8 executeThumbUnconditionalBranch(ThumbInstruction& ins);
	u8 executeThumbLoadFromPool(ThumbInstruction& ins);
	u8 executeThumbLoadStoreRegisterOffset(ThumbInstruction& ins);
	u8 executeThumbLoadStoreWordByteImmOffset(ThumbInstruction& ins);
	u8 executeThumbLoadStoreHalfwordImmOffset(ThumbInstruction& ins);
	u8 executeThumbShiftByImm(ThumbInstruction& ins);
	u8 executeThumbDataProcessingImm(ThumbInstruction& ins);
	u8 executeThumbDataProcessingReg(ThumbInstruction& ins);
	u8 executeThumbAddSubReg(ThumbInstruction& ins);
	u8 executeThumbAddSubImm(ThumbInstruction& ins);
	u8 executeThumbLoadStoreMultiple(ThumbInstruction& ins);
	u8 executeThumbBranchExchange(ThumbInstruction& ins);
	u8 executeThumbSpecialDataProcessing(ThumbInstruction& ins);
	u8 executeThumbLoadStoreStack(ThumbInstruction& ins);
	u8 executeThumbAddSPOrPC(ThumbInstruction& ins);
	u8 executeThumbMisc(ThumbInstruction& ins);

	u8 handleUndefinedThumbIns(ThumbInstruction& ins);

private:
	void mapArmOpcodes();
	void mapThumbOpcodes();

	//Arm Instructions

	//Data processing
	u8 opMOV(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opADD(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opAND(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opEOR(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opSUB(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opRSB(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opADC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opSBC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opRSC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opTST(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opTEQ(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opCMP(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opCMN(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opORR(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opBIC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opMVN(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);

	//Branches
	u8 opB(ArmInstruction& ins);
	u8 opBL(ArmInstruction& ins);
	u8 opBX(ArmInstruction& ins);
	u8 opSWI(ArmInstruction& ins);

	//Misc Load/Stores
	u8 opSTRH(ArmInstruction &ins, RegisterID rd, u32 address);
	u8 opLDRH(ArmInstruction& ins, RegisterID rd, u32 address);
	u8 opLDRSB(ArmInstruction& ins, RegisterID rd, u32 address);
	u8 opLDRSH(ArmInstruction& ins, RegisterID rd, u32 address);
	u8 opSWP(ArmInstruction& ins, RegisterID rd, RegisterID rn);
	u8 opSWPB(ArmInstruction& ins, RegisterID rd, RegisterID rn);

	//Load/Stores
	u8 opLDRB(ArmInstruction& ins, RegisterID rd, u32 address);
	u8 opLDR(ArmInstruction& ins, RegisterID rd, u32 address);
	u8 opSTRB(ArmInstruction& ins, RegisterID rd, u32 address);
	u8 opSTR(ArmInstruction& ins, RegisterID rd, u32 address);

	//Status Register access instructions
	u8 opMRS(ArmInstruction& ins);
	u8 opMSR(ArmInstruction& ins, u32 value);

	//Multiplies
	u8 opMUL(ArmInstruction &ins, RegisterID rd, RegisterID rm, RegisterID rs, bool flags);
	u8 opMLA(ArmInstruction& ins, RegisterID rd, RegisterID rm, RegisterID rs, bool flags);
	u8 opSMLAL(ArmInstruction& ins, RegisterID rdhi, RegisterID rdlo, RegisterID rm, RegisterID rs, bool flags);
	u8 opSMULL(ArmInstruction& ins, RegisterID rdhi, RegisterID rdlo, RegisterID rm, RegisterID rs, bool flags);
	u8 opUMLAL(ArmInstruction& ins, RegisterID rdhi, RegisterID rdlo, RegisterID rm, RegisterID rs, bool flags);
	u8 opUMULL(ArmInstruction& ins, RegisterID rdhi, RegisterID rdlo, RegisterID rm, RegisterID rs, bool flags);

	//Thumb instructions

	u8 thumbOpBCond(ThumbInstruction& ins);
	u8 thumbOpB(ThumbInstruction& ins);
	u8 thumbOpBL(ThumbInstruction& ins);
	u8 thumbOpLDRPool(ThumbInstruction& ins);
	u8 thumbOpLDRStack(ThumbInstruction& ins);
	u8 thumbOpSTRStack(ThumbInstruction& ins);
	u8 thumbOpASR(ThumbInstruction& ins, u8 immediate5);
	u8 thumbOpASR(ThumbInstruction& ins, RegisterID rs, RegisterID rd);
	u8 thumbOpLSL(ThumbInstruction& ins, u8 immediate5);
	u8 thumbOpLSL(ThumbInstruction& ins, RegisterID rs, RegisterID rd);
	u8 thumbOpLSR(ThumbInstruction& ins, u8 immediate5);
	u8 thumbOpLSR(ThumbInstruction& ins, RegisterID rs, RegisterID rd);
	u8 thumbOpMOV(ThumbInstruction& ins, RegisterID rm, RegisterID rd);
	u8 thumbOpMOV(ThumbInstruction& ins, u8 immediate);
	u8 thumbOpMUL(ThumbInstruction& ins, RegisterID rm, RegisterID rd);
	u8 thumbOpNEG(ThumbInstruction& ins, RegisterID rm, RegisterID rd);
	u8 thumbOpORR(ThumbInstruction& ins, RegisterID rm, RegisterID rd);
	u8 thumbOpMVN(ThumbInstruction& ins, RegisterID rm, RegisterID rd);
	u8 thumbOpROR(ThumbInstruction& ins, RegisterID rs, RegisterID rd);
	u8 thumbOpADC(ThumbInstruction& ins, RegisterID rm, RegisterID rd);
	u8 thumbOpADD(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd); //register add
	u8 thumbOpADD(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate); //3 bit immediate
	u8 thumbOpADD(ThumbInstruction& ins, RegisterID rd, u8 immediate); //large immediate
	u8 thumbOpADD(ThumbInstruction& ins, RegisterID rm, RegisterID rd); //hi registers
	u8 thumbOpADD(ThumbInstruction& ins, RegisterID rd, bool pc); //add sp/pc
	u8 thumbOpADD(ThumbInstruction& ins, u8 immediate7); //increment SP
	u8 thumbOpAND(ThumbInstruction& ins, RegisterID rm, RegisterID rd);
	u8 thumbOpCMN(ThumbInstruction& ins, RegisterID rm, RegisterID rn);
	u8 thumbOpCMP(ThumbInstruction& ins, RegisterID rm, RegisterID rn, bool hiRegisters); //register
	u8 thumbOpCMP(ThumbInstruction& ins, u8 immediate); //immediate
	u8 thumbOpEOR(ThumbInstruction& ins, RegisterID rm, RegisterID rd);
	u8 thumbOpSBC(ThumbInstruction& ins, RegisterID rm, RegisterID rd);
	u8 thumbOpSUB(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd);
	u8 thumbOpSUB(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate);
	u8 thumbOpSUB(ThumbInstruction& ins, RegisterID rd, u8 immediate); //large immediate
	u8 thumbOpSUB(ThumbInstruction& ins, u8 immediate7); //decrement SP
	u8 thumbOpBIC(ThumbInstruction& ins, RegisterID rm, RegisterID rd);
	u8 thumbOpTST(ThumbInstruction& ins, RegisterID rm, RegisterID rn);
	u8 thumbOpSTMIA(ThumbInstruction& ins);
	u8 thumbOpLDMIA(ThumbInstruction& ins);
	u8 thumbOpBX(ThumbInstruction& ins);
	u8 thumbOpSWI(ThumbInstruction& ins);

	u8 thumbOpLDR(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate5); //immediate offset
	u8 thumbOpLDR(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd); //register offset
	u8 thumbOpLDRB(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate5);
	u8 thumbOpLDRB(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd);
	u8 thumbOpLDRSB(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd);
	u8 thumbOpLDRSH(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd);
	u8 thumbOpLDRH(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate5); //immediate offset
	u8 thumbOpLDRH(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd); //register offset
	u8 thumbOpSTR(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate5);
	u8 thumbOpSTR(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd);
	u8 thumbOpSTRB(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate5);
	u8 thumbOpSTRB(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd);
	u8 thumbOpSTRH(ThumbInstruction& ins, RegisterID rn, RegisterID rd, u8 immediate5);
	u8 thumbOpSTRH(ThumbInstruction& ins, RegisterID rm, RegisterID rn, RegisterID rd);

	u8 thumbOpPUSH(ThumbInstruction& ins);
	u8 thumbOpPOP(ThumbInstruction& ins);


public:
	State state;
	ProcessorMode mode;
	/*
		in THUMB mode only R0-R7 (Lo registers) may be accessed freely,
		while R8-R12 and up (Hi registers) can be accessed 
		only by some instructions.
	*/
	Register registers[NUM_REGISTERS]; //R0 - R12
	Register registersFiq[NUM_REGISTERS_FIQ]; //R8 - R14

	u32 SP; //R13
	//Stores return addr when calling subroutine or branch instr
	u32 LR; //R14
	u32 R15; //contains PC
	u32 CPSR;
	u32 SPSR;
	u32 SPSR_fiq;
	u32 SPSR_svc;
	u32 SPSR_abt;
	u32 SPSR_irq;
	u32 SPSR_und;

	//FIQ
	u32 SP_fiq;
	u32 LR_fiq;

	//IRQ
	u32 SP_irq;
	u32 LR_irq;

	//SVC
	u32 SP_svc;
	u32 LR_svc;

	//ABT
	u32 SP_abt;
	u32 LR_abt;

	//UND
	u32 SP_und;
	u32 LR_und;

	AddressingMode1 addrMode1;
	AddressingMode2 addrMode2;
	AddressingMode3 addrMode3;
	AddressingMode4 addrMode4;

	std::array<std::function<u8(ArmInstruction&)>, 4096> armlut;
	std::array<std::function<u8(ThumbInstruction&)>, 256> thumblut;

	u32 armpipeline[2];
	u16 thumbpipeline[2];

	u32 cycles = 0;
	u32 currentExecutingArmOpcode;
	u16 currentExecutingThumbOpcode;
	MemoryBus* mbus;
};