#pragma once
#include "Utils.h"

struct CpuState {
	u32 opcode;
	u32 regs[13];
	u32 sp;
	u32 lr;
	u32 r15;
	u32 cpsr;
	u32 spsr;
};

class Ringbuffer {
public:
	Ringbuffer(u32 size);
	~Ringbuffer();
	void print();
	void add(CpuState state);
	CpuState* get();

	CpuState* buffer;
	u32 currentIndex;
	u32 size;
};