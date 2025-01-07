#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__


#ifdef FLAVOUR_PICORV32
#define INTERRUPTS_REGS_N (31+4)		//32 main registers and 4 interrupt registers
#define INT_NMI		0x08
#define INT_IRQ		0x10
#define INT_DEBUG	0x20
#define INT_ALL		0x38
#endif
#ifdef FLAVOUR_HAZARD3
#define INTERRUPTS_REGS_N (31+4)		//32 main registers and 4 interrupt registers
#define INT_IRQ		0x01
#define INT_DEBUG	0x02
#define INT_NMI		0x04
#define INT_ALL		0x07
#endif

#ifndef __ASSEMBLER__
#include <stdint.h>

extern uint32_t interrupts_init();
extern uint32_t interrupts_disable(int mask);
extern uint32_t interrupts_regs[INTERRUPTS_REGS_N];
#endif


#endif