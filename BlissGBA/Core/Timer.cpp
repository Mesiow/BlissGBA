#include "Timer.h"
#include "../Memory/MemoryBus.h"

TimerController::TimerController(MemoryBus* mbus)
	:mbus(mbus)
{
	for (u32 i = 0; i < 4; i++) {
		timers[i].tmcnth = 0;
		timers[i].tmcntl = 0;
		timers[i].counter = 0;
		timers[i].subcounter = 0;
		timers[i].overflow = false;
	}

	prescaler[0] = 1;
	prescaler[1] = 64;
	prescaler[2] = 256;
	prescaler[3] = 1024;
}

void TimerController::handleTimers()
{
	for (u32 i = 0; i < 4; i++) {
		u16 old_counter = timers[i].counter;
		timers[i].overflow = false;

		bool start = timers[i].control.start;
		if (start) {
			//If countup timing enabled(not used for timer 0)
			//then the counter is incremented each time the previous timer
			//overflows
			if (timers[i].control.countup && i != 0) {
				if (timers[i - 1u].overflow) {
					++timers[i].counter;
				}
			}
			else {
				++timers[i].subcounter;
				if (timers[i].subcounter >= prescaler[timers[i].control.prescaler]) {
					timers[i].subcounter = 0;
					++timers[i].counter;
				}
			}
			
			if (old_counter == 0xFFFF && timers[i].counter == 0) {
				timers[i].overflow = true;
				//reload value loaded into counter upon overflow
				timers[i].counter = timers[i].tmcntl;
				if (timers[i].control.irq) {
					switch (i) {
						case 0: requestInterrupt(TIMER0_INT); break;
						case 1: requestInterrupt(TIMER1_INT); break;
						case 2: requestInterrupt(TIMER2_INT); break;
						case 3: requestInterrupt(TIMER3_INT); break;
					}
				}
			}
		}
	}
}

void TimerController::requestInterrupt(u16 interrupt)
{
	u16 irq_flag = mbus->mmio.readIF();
	irq_flag = setBit(irq_flag, interrupt);
	mbus->mmio.writeIF(irq_flag);
}

void TimerController::setControl(eTimer timer, u16 value)
{
	bool old_start_bit = timers[(u8)timer].control.start;

	/*printf("Old:\n");
	printf("control: 0x%04X\n", timers[(u8)timer].tmcnth);
	printf("start: 0x%04X\n", old_start_bit);
	printf("value: 0x%04X\n\n", value);*/

	timers[(u8)timer].tmcnth = value;
	timers[(u8)timer].subcounter = 0;

	/*printf("New:\n");
	printf("control: 0x%04X\n", timers[(u8)timer].tmcnth);
	printf("start: 0x%04X\n", timers[(u8)timer].control.start);
	printf("value: 0x%04X\n\n", value);*/

	if ((old_start_bit == 0x0) && timers[(u8)timer].control.start) {
		
		timers[(u8)timer].counter = timers[(u8)timer].tmcntl;
	}
}

void TimerController::setTimerReload(eTimer timer, u16 value)
{
	timers[(u8)timer].tmcntl = value;
}

u16 TimerController::getTimerCounter(eTimer timer)
{
	return timers[(u8)timer].counter;
}

u16 TimerController::getTimerReload(eTimer timer)
{
	return timers[(u8)timer].tmcntl;
}

u16 TimerController::getTimerControlRegister(eTimer timer)
{
	switch (timer) {
		case eTimer::TM0: return timers[0].tmcnth; break;
		case eTimer::TM1: return timers[1].tmcnth; break;
		case eTimer::TM2: return timers[2].tmcnth; break;
		case eTimer::TM3: return timers[3].tmcnth; break;
	}
}
