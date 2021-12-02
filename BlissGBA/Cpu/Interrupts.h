#pragma once

//Interrupt Control
#define IME 0x4000208
#define IE 0x4000200
#define IF 0x4000202
#define IRQ_VECTOR 0x18

#define VBLANK_INT 0
#define HBLANK_INT 1
#define VCOUNT_INT 2
#define TIMER0_INT 3
#define TIMER1_INT 4
#define TIMER2_INT 5
#define TIMER3_INT 6
#define SERIAL_INT 7
#define DMA0_INT 8
#define DMA1_INT 9
#define DMA2_INT 10
#define DMA3_INT 11
#define KEYPAD_INT 12