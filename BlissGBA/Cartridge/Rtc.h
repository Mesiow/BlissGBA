#pragma once
#include "../Core/Interrupts.h"
#include <time.h>

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

enum class DateTimeByte : u8 {
	Year = 0,
	Month = 1,
	Day = 2,
	DayOfWeek = 3,
	Hour = 4,
	Min = 5,
	Sec = 6
};

struct RtcDevice {
	RtcDevice(MemoryBus* mbus);
	void reset();
	u8 read(GpioAddress address);
	void write(GpioAddress address, u8 value);
	void handleCommandBegin(u8 command_byte);
	void handleCommandSetup();
	void handleReadCommand();
	void handleWriteCommand();

	void setDateTime(DateTimeByte byte, u8 value);
	u8 getDateTime(DateTimeByte byte);

	time_t curr_time;
	tm* local_time;
	GpioInterface gpio;
	RtcRegisters rtc_regs;

	bool transfer_in_progress = false;
	u8 command_byte = 0;
	u8 sample_bit = 0;
	u8 register_length_bytes = 0;
	u8 current_n_byte = 0;
	s8 sample_reg_bit = 0;
	bool write_active = false;
	bool read_active = false;

	MemoryBus* mbus;
};
