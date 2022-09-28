#pragma once
#include "../../Utils/Utils.h"

//eeprom accessed on cartridge wait state 2
//D000000 - 0DFFFFFF according to eeprom article
struct Eeprom {
	u8 size : 1; //1 = 512 byte , 0 = 8kb
	bool begin_read;
	bool begin_write;
};
