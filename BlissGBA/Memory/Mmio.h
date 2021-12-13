#pragma once
#include "../Utils.h"

class GeneralMemory;

struct Mmio {
	Mmio(GeneralMemory *gm);
	void setIF(u16 value);

	GeneralMemory* gm;
};