#include "Ringbuffer.h"

Ringbuffer::Ringbuffer(u32 size)
{
	this->size = size;
	currentIndex = 0;
	buffer = new CpuState[size];
}

Ringbuffer::~Ringbuffer()
{
	delete buffer;
}

void Ringbuffer::print()
{
	for (u32 i = 0; i < size; i++) {
		printf("Opcode: 0x%08X\n", buffer[i].opcode);
		for (u32 j = 0; j < 13; j++) {
			printf("R%d: ", j);
			printf("0x%08X\n", buffer[i].regs[j]);
		}
		printf("SP: 0x%08X\n", buffer[i].sp);
		printf("LR: 0x%08X\n", buffer[i].lr);
		printf("R15: 0x%08X\n", buffer[i].r15);
		printf("CPSR: 0x%08X\n", buffer[i].cpsr);
		printf("SPSR: 0x%08X\n", buffer[i].spsr);

		printf("\n\n\n");
	}
}

void Ringbuffer::add(CpuState state)
{
	buffer[currentIndex] = state;
	currentIndex++;

	if (currentIndex >= size) currentIndex = 0;
}

CpuState* Ringbuffer::get()
{
	CpuState* ret_buffer = new CpuState[size];
	for (u32 i = 0; i < size; i++) {
		ret_buffer[i] = buffer[(i + currentIndex) % size];
	}

	return ret_buffer;
}
