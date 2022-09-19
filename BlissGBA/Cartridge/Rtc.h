#pragma once
#include "../Core/Interrupts.h"

#define IN 0
#define OUT 1

enum class GpioAddress {
	Data = 0x80000C4,
	Direction = 0x80000C6,
	Control = 0x80000C8
};

struct GpioInterface {
	u8 data_register; //4 bits
	u8 direction_register; //4 bits
	u8 control_register; //1 bit wide
};

struct RtcRegisters {
	u8 control;
	u8 date_time[7];
	u32 time : 24;
	u8 reset;
	u8 irq;
};

struct RtcDevice {
	RtcDevice(MemoryBus* mbus);
	u8 read(GpioAddress address);
	void write(GpioAddress address, u8 value);
	void handleCommand(u8 command_byte);

	GpioInterface gpio;
	RtcRegisters rtc_regs;

	bool transfer_in_progress = false;
	u8 command_byte = 0;
	u8 sample_bit = 0;

	MemoryBus* mbus;
};
