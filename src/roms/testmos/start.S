		#include "custom_ops.S"

start:
		li	x2,0xFFFFFFF8


		picorv32_maskirq_insn(x2, x2)

		la 	sp,0x10000		// setup set stack in ChipRAM at 00010000
		
		j	main


handle_irq:

		picorv32_setq_insn(q2, ra)	// save return address
		picorv32_setq_insn(q3, sp)	// save stack pointer

		la x1, irq_regs

		// q0 is set to return address of interrupt
		picorv32_getq_insn(x2, q0)
		sw x2,   32*4(x1)

		// q1 is set to pending interrupts
		picorv32_getq_insn(x2, q1)
		sw x2,   33*4(x1)

		// saved x1/ra above
		picorv32_getq_insn(x2, q2)
		sw x2,   34*4(x1)
		sw x2,   1*4(x1)

		// saved x2/sp above
		picorv32_getq_insn(x2, q3)
		sw x2,   35*4(x1)
		sw x2,   2*4(x1)

		sw x3,   3*4(x1)
		sw x4,   4*4(x1)
		sw x5,   5*4(x1)
		sw x6,   6*4(x1)
		sw x7,   7*4(x1)
		sw x8,   8*4(x1)
		sw x9,   9*4(x1)
		sw x10, 10*4(x1)
		sw x11, 11*4(x1)
		sw x12, 12*4(x1)
		sw x13, 13*4(x1)
		sw x14, 14*4(x1)
		sw x15, 15*4(x1)
		sw x16, 16*4(x1)
		sw x17, 17*4(x1)
		sw x18, 18*4(x1)
		sw x19, 19*4(x1)
		sw x20, 20*4(x1)
		sw x21, 21*4(x1)
		sw x22, 22*4(x1)
		sw x23, 23*4(x1)
		sw x24, 24*4(x1)
		sw x25, 25*4(x1)
		sw x26, 26*4(x1)
		sw x27, 27*4(x1)
		sw x28, 28*4(x1)
		sw x29, 29*4(x1)
		sw x30, 30*4(x1)
		sw x31, 31*4(x1)

		jal irq

		//;;;;;TODO!


		.section reset, "x", @progbits
reset_hw:
j		start
		.section irq, "x", @progbits
irq_hw:
j		handle_irq


