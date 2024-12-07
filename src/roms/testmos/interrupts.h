#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <stdint.h>

extern void interrupts_disable(int mask);
extern uint32_t interrupts_regs[32+4];

#define INT_NMI		0x08
#define INT_IRQ		0x10
#define INT_DEBUG	0x20

#endif