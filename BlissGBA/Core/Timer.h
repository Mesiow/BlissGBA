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

class MemoryBus;

enum class Timer : u8 {
	TM0 = 0,
	TM1 = 1,
	TM2 = 2,
	TM3 = 3
};

struct TimerController {
	TimerController(MemoryBus* mbus);
	void handleTimers(u32 cycles);
	void enableTimer(Timer timer);
	void requestInterrupt(u16 interrupt);

	void setTimerCounter(Timer timer, u16 value);
	void setTimerReload(Timer timer, u16 value);
	u16 getTimerCounter(Timer timer);
	u16 getTimerReload(Timer timer);

	MemoryBus* mbus;
	u16 timerCounterValues[4];
	u16 timerReloadValues[4];
	bool enabledTimers[4];
};