#ifndef __DEICE_H__
#define __DEICE_H__


#define ACIA_STAT_RDRF 0x01
#define ACIA_STAT_TDRE 0x02

#define FN_GET_STAT			0xFF    // reply with device info
#define FN_READ_MEM			0xFE    // reply with data
#define FN_WRITE_M			0xFD    // reply with status (+/-)
#define FN_READ_RG			0xFC    // reply with registers
#define FN_WRITE_RG			0xFB    // reply with status
#define FN_RUN_TARG			0xFA    // reply (delayed) with registers
#define FN_SET_BYTES		0xF9    // reply with data (truncate if error)
#define FN_IN				0xF8    // input from port
#define FN_OUT				0xF7    // output to port
#define FN_MIN				0xF7    // MINIMUM RECOGNIZED FUNCTION CODE
#define FN_ERROR			0xF0    // error reply to unknown op-code

//target statuses
#define TS_RUNNING			0
#define TS_BP				1
#define TS_TRACE			2
#define TS_ILLEGAL			3

#ifdef FLAVOUR_PICORV32
#define TS_RV_TIMER			0x10
#define TS_RV_IRQ			0x11
#define TS_RV_NMI			0x12
#define TS_RV_DEBUG			0x13
#define TS_RV_CALL			0x14
#define TS_RV_BUSERROR		0x15
#define TS_RV_UNKNOWN		0x20
#endif

#ifdef FLAVOUR_HAZARD3
#define TS_H3_SOFT			0x10
#define TS_H3_TIMER			0x11
#define TS_H3_DEBUG			0x13

#define TS_H3_UNKNOWN		0x20
#define TS_H3_UNKNOWN_EX	0x30
#endif



//TODO:we may change this depending on host extra regs have different meanings
#define DEICE_INTERRUPT_REGISTERS 35	// 31 + PC, Q1, Q2, Q3

//indexes within the array of 32bit ints in register block / stack frames

#define deice_REG_IX_x1   0
#define deice_REG_IX_x2   1
#define deice_REG_IX_x3   2
#define deice_REG_IX_x4   3
#define deice_REG_IX_x5   4
#define deice_REG_IX_x6   5
#define deice_REG_IX_x7   6
#define deice_REG_IX_x8   7
#define deice_REG_IX_x9   8
#define deice_REG_IX_x10  9
#define deice_REG_IX_x11 10
#define deice_REG_IX_x12 11
#define deice_REG_IX_x13 12
#define deice_REG_IX_x14 13
#define deice_REG_IX_x15 14
#define deice_REG_IX_x16 15
#define deice_REG_IX_x17 16
#define deice_REG_IX_x18 17
#define deice_REG_IX_x19 18
#define deice_REG_IX_x20 19
#define deice_REG_IX_x21 20
#define deice_REG_IX_x22 21
#define deice_REG_IX_x23 22
#define deice_REG_IX_x24 23
#define deice_REG_IX_x25 24
#define deice_REG_IX_x26 25
#define deice_REG_IX_x27 26
#define deice_REG_IX_x28 27
#define deice_REG_IX_x29 28
#define deice_REG_IX_x30 29
#define deice_REG_IX_x31 30

#define deice_REG_IX_ra		deice_REG_IX_x1
#define deice_REG_IX_sp		deice_REG_IX_x2
#define deice_REG_IX_gp		deice_REG_IX_x3
#define deice_REG_IX_tp		deice_REG_IX_x4
#define deice_REG_IX_t0		deice_REG_IX_x5
#define deice_REG_IX_t1		deice_REG_IX_x6
#define deice_REG_IX_t2		deice_REG_IX_x7
#define deice_REG_IX_s0		deice_REG_IX_x8
#define deice_REG_IX_s1		deice_REG_IX_x9
#define deice_REG_IX_a0		deice_REG_IX_x10
#define deice_REG_IX_a1		deice_REG_IX_x11
#define deice_REG_IX_a2		deice_REG_IX_x12
#define deice_REG_IX_a3		deice_REG_IX_x13
#define deice_REG_IX_a4		deice_REG_IX_x14
#define deice_REG_IX_a5		deice_REG_IX_x15
#define deice_REG_IX_a6		deice_REG_IX_x16
#define deice_REG_IX_a7		deice_REG_IX_x17
#define deice_REG_IX_s2		deice_REG_IX_x18
#define deice_REG_IX_s3		deice_REG_IX_x19
#define deice_REG_IX_s4		deice_REG_IX_x20
#define deice_REG_IX_s5		deice_REG_IX_x21
#define deice_REG_IX_s6		deice_REG_IX_x22
#define deice_REG_IX_s7		deice_REG_IX_x23
#define deice_REG_IX_s8		deice_REG_IX_x24
#define deice_REG_IX_s9		deice_REG_IX_x25
#define deice_REG_IX_s10	deice_REG_IX_x26
#define deice_REG_IX_s11	deice_REG_IX_x27
#define deice_REG_IX_t3		deice_REG_IX_x28
#define deice_REG_IX_t4		deice_REG_IX_x29
#define deice_REG_IX_t5		deice_REG_IX_x30
#define deice_REG_IX_t6		deice_REG_IX_x31


#define deice_REG_OFF(rn)	(4*deice_REG_IX_ ## rn)

#ifdef FLAVOUR_HAZARD3
#define deice_REG_IX_mepc			31
#define deice_REG_IX_mcause			32
#define deice_REG_IX_meicontext		33
#define deice_REG_IX_mstatus		34

#endif

#ifndef __ASSEMBLER__
#include <stdint.h>
/* defines for UART TODO: move to hardware include?*/
#define ACIA_DATA (*((volatile uint8_t*)0xFFFFFE09))
#define ACIA_STAT (*((volatile uint8_t*)0xFFFFFE08))
#define SER_ULA   (*((volatile uint8_t*)0xFFFFFE10))

extern void deice_init(void);
extern void deice_print_char(char c);
extern void deice_print_str(const char *str);
extern void deice_enter(uint8_t status, uint32_t * registers);
#endif

#endif