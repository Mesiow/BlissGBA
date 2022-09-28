#pragma once
#include "../../Utils/Utils.h"
#include <unordered_map>

enum class SaveType : u8{
    EEPROM_4k,
    EEPROM_4k_alt,
    EEPROM_8k,
    EEPROM_8k_alt,
    Flash_512k_Atmel_RTC,
    Flash_512k_Atmel,
    Flash_512k_SST_RTC,
    Flash_512k_SST,
    Flash_512k_Panasonic_RTC,
    Flash_512k_Panasonic,
    Flash_1M_Macronix_RTC,
    Flash_1M_Macronix,
    Flash_1M_Sanyo_RTC,
    Flash_1M_Sanyo,
    SRAM_256K,
    NONE,
    UNKNOWN
};

class GamePak;

SaveType detectSavetype(GamePak& pak);
SaveType lookupSavetype(GamePak& pak);
void setupSaveDatabase();