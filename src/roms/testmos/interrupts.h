#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <stdint.h>

extern void interrupts_disable(int mask);
#define INTERRUPTS_REGS_N (31+4)		//32 main registers and 4 interrupt registers
extern uint32_t interrupts_regs[INTERRUPTS_REGS_N];

#define INT_NMI		0x08
#define INT_IRQ		0x10
#define INT_DEBUG	0x20

#endif