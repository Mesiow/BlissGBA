#pragma once
#include <cstdint>
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>

using u8 = uint8_t;
using s8 = int8_t;
using u16 = uint16_t;
using s16 = int16_t;
using u32 = uint32_t;
using s32 = int32_t;

u8 setBit(u8 val, u8 bitToSet);
u8 resetBit(u8 val, u8 bitToReset);
u8 testBit(u8 val, u8 bitToTest);