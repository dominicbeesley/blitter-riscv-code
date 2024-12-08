		#include "custom_ops.S"


		.globl interrupts_disable
		.globl interrupts_handle
		.globl interrupts_regs

		.bss
		.align(4)
//int interrupts_regs[32+4];						// 32 caller regs + q0..3
interrupts_regs:
		.space (32+4)*4


		.text
interrupts_disable:

		// ensure bus, instr, timer
		andi	a0, a0, 7

		picorv32_maskirq_insn(a0, a0)

		ret


interrupts_handle:

		picorv32_setq_insn(q2, ra)	// save return address
		picorv32_setq_insn(q3, sp)	// save stack pointer

		la x1, interrupts_regs

		// q0 is set to return address of interrupt
		picorv32_getq_insn(x2, q0)
		sw x2,   31*4(x1)

		// q1 is set to pending interrupts
		picorv32_getq_insn(x2, q1)
		sw x2,   32*4(x1)

		// saved x1/ra above
		picorv32_getq_insn(x2, q2)
		sw x2,   33*4(x1)
		sw x2,   0*4(x1)

		// saved x2/sp above
		picorv32_getq_insn(x2, q3)
		sw x2,   34*4(x1)
		sw x2,   1*4(x1)


		sw x3,   2*4(x1)
		sw x4,   3*4(x1)
		sw x5,   4*4(x1)
		sw x6,   5*4(x1)
		sw x7,   6*4(x1)
		sw x8,   7*4(x1)
		sw x9,   8*4(x1)
		sw x10, 9*4(x1)
		sw x11, 10*4(x1)
		sw x12, 11*4(x1)
		sw x13, 12*4(x1)
		sw x14, 13*4(x1)
		sw x15, 14*4(x1)
		sw x16, 15*4(x1)
		sw x17, 16*4(x1)
		sw x18, 17*4(x1)
		sw x19, 18*4(x1)
		sw x20, 19*4(x1)
		sw x21, 20*4(x1)
		sw x22, 21*4(x1)
		sw x23, 22*4(x1)
		sw x24, 23*4(x1)
		sw x25, 24*4(x1)
		sw x26, 25*4(x1)
		sw x27, 26*4(x1)
		sw x28, 27*4(x1)
		sw x29, 28*4(x1)
		sw x30, 29*4(x1)
		sw x31, 30*4(x1)

		li	t0,0x44434241
		la	t1,0xFFFF7C00
		sw	t0,0(t1)

		jal deice_enter

		//;;;;;TODO!

