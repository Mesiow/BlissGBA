#include "Timer.h"
#include "../Memory/MemoryBus.h"

TimerController::TimerController(MemoryBus* mbus)
	:mbus(mbus)
{
	memset(timerCounterValues, 0x0, 4);
	memset(timerReloadValues, 0x0, 4);
	memset(enabledTimers, 0x0, 4);
}

void TimerController::handleTimers(u32 cycles)
{
	u16 timer_cnt_0 = mbus->mmio.readTMCNTH(TM0CNT_H);
	u16 timer_cnt_1 = mbus->mmio.readTMCNTH(TM1CNT_H);
	u16 timer_cnt_2 = mbus->mmio.readTMCNTH(TM2CNT_H);
	u16 timer_cnt_3 = mbus->mmio.readTMCNTH(TM3CNT_H);


	if (enabledTimers[(u8)Timer::TM0]) {

	}
	if (enabledTimers[(u8)Timer::TM1]) {

	}
	if (enabledTimers[(u8)Timer::TM2]) {

	}
	if (enabledTimers[(u8)Timer::TM3]) {

	}
}

void TimerController::enableTimer(Timer timer)
{
	enabledTimers[(u8)timer] = true;
}

void TimerController::requestInterrupt(u16 interrupt)
{
	u16 irq_flag = mbus->mmio.readIF();
	irq_flag = setBit(irq_flag, interrupt);
	mbus->mmio.writeIF(irq_flag);
}

void TimerController::setTimerCounter(Timer timer, u16 value)
{
	timerCounterValues[(u8)timer] = value;
}

void TimerController::setTimerReload(Timer timer, u16 value)
{
	timerReloadValues[(u8)timer] = value;
}

u16 TimerController::getTimerCounter(Timer timer)
{
	return timerCounterValues[(u8)timer];
}

u16 TimerController::getTimerReload(Timer timer)
{
	return timerReloadValues[(u8)timer];
}
