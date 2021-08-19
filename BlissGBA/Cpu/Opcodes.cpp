#include "Arm.h"
#include "../Memory/MemoryBus.h"



void Arm::mapArmOpcodes()
{
	for (s32 i = 0; i < 4096; i++) {
		//SWI
		if (i & 0xF00 == 0xF00) {
			armlut[i] = b(&Arm::opSWI);
		}
		//Branch
		else if ((i >> 8) == 0b1010) {
			armlut[i] = b(&Arm::opB);
		}
		//Branch with link
		else if ((i >> 8) == 0b1011) {
			armlut[i] = b(&Arm::opBL);
		}
		else if (i == 0b000100100001) {
			armlut[i] = b(&Arm::opBX);
		}
		//Addressing Mode 1 immediate with flags(S bit)
		else if (((i >> 9) == 0b001) && ((i >> 4) & 0x1) == 1) {
			armlut[i] = b(&Arm::executeDataProcessingImmFlags);
		}
		//Addressing Mode 1 immediate
		else if ((i >> 9) == 0b001) {
			armlut[i] = b(&Arm::executeDataProcessingImm);
		}
		//Addressing Mode 1 immediate shift with flags(S bit)
		else if (((i >> 9) == 0) && ((i & 0x1) == 0) && ((i >> 4) & 0x1 == 0x1)) {
			armlut[i] = b(&Arm::executeDataProcessingImmShiftFlags);
		}
		//Addressing Mode 1 immediate shift
		else if (((i >> 9) == 0) && ((i & 0x1) == 0)) {
			armlut[i] = b(&Arm::executeDataProcessingImmShift);
		}
		//Addressing Mode 1 register shift with flags(S bit)
		else if (((i >> 9) == 0) && ((i >> 4) & 0x1) == 0x1) {
			armlut[i] = b(&Arm::executeDataProcessingRegShiftFlags);
		}
		else if ((i >> 9) == 0) {
			armlut[i] = b(&Arm::executeDataProcessingRegShift);
		}
		
	}
}

void Arm::mapThumbOpcodes() 
{

}
