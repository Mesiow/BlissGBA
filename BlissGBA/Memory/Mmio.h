#pragma once

class GeneralMemory;

struct Mmio {
	Mmio(GeneralMemory *gm);
	void setIF(u16 value);

	GeneralMemory* gm;
};