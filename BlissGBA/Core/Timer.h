#pragma once
#include "../Utils/Utils.h"
#include "Interrupts.h"

#define TM0CNT_L 0x4000100
#define TM1CNT_L 0x4000104
#define TM2CNT_L 0x4000108
#define TM3CNT_L 0x400010C

#define TM0CNT_H 0x4000102
#define TM1CNT_H 0x4000106
#define TM2CNT_H 0x400010A
#define TM3CNT_H 0x400010E

#define FREQ 16780000
#define FREQ_64 262210
#define FREQ_256 65536
#define FREQ_1024 16384

class MemoryBus;

enum class eTimer : u8 {
	TM0 = 0,
	TM1 = 1,
	TM2 = 2,
	TM3 = 3
};

struct Timer {
	u16 tmcntl;
	union {
		struct {
			u16 prescaler : 2;
			bool countup : 1;
			u16 unused_3_5 : 3;
			bool irq : 1;
			bool start : 1;
			u16 unused_8_15 : 8;
		}control;
		u16 tmcnth;
	};
	u16 counter;
	u16 subcounter;
	bool overflow;
};

struct TimerController {
	TimerController(MemoryBus* mbus);
	void handleTimers();
	void requestInterrupt(u16 interrupt);

	void setControl(eTimer timer, u16 value);
	void setTimerReload(eTimer timer, u16 value);
	u16 getTimerCounter(eTimer timer);
	u16 getTimerReload(eTimer timer);
	u16 getTimerControlRegister(eTimer timer);

	MemoryBus* mbus;
	Timer timers[4];
	u16 cycleCounter = 0;

	u32 prescaler[4];
};