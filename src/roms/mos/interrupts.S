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
		andi	a0, a0, ~7

		picorv32_maskirq_insn(a0, a0)

		ret

		.bss
		.align(4)
		.globl mos_stack_base
		.globl mos_stack_top
mos_stack_base:
		.space (0x1000)
mos_stack_top:
mos_stack_len = mos_stack_top-mos_stack_base
		.text

		//TODO: move this to own file
mos_enter:
		// we should be entered with
		// q0 = address after ecall
		// q1 = interrupt status - we will cancel bit 2
		// q2 = callers ra
		// q3 = callers sp

		// if q3 isn't in MOS stack then set to MOS stack - allow MOS to reenter, maybe change this?
		picorv32_getq_insn(sp, q3)
		la		ra,mos_stack_base
		blt		sp,ra,.news
		la		ra,mos_stack_top
		blt		sp,ra,.nonews

		// we weren't in the mos set to MOS stack
.news:	la		sp,mos_stack_top
.nonews:
		addi	sp,sp,-(10*4)			// reserved space on stack, 7 arguments, ra, sp, pc
		picorv32_getq_insn(ra, q3)		// get back caller's sp and push
		sw		ra,0(sp)
		picorv32_getq_insn(ra, q2)		// get back caller's ra and push
		sw		ra,4(sp)
		picorv32_getq_insn(ra, q0)		// get back caller's pc and push
		sw		ra,8(sp)
		// reset the illegal instruction bit in q1
		picorv32_getq_insn(ra, q1)
		andi	ra,ra,~2
		picorv32_setq_insn(q1, ra)
		la		ra,.cont
		picorv32_setq_insn(q0, ra)
		picorv32_retirq_insn()			// continue at .cont but allow other irqs to re-fire
////////////////////////////////////////////////////////////
.cont:	
		sw		a0,12(sp)
		sw		a1,16(sp)	
		sw		a2,20(sp)	
		sw		a3,24(sp)	
		sw		a4,28(sp)	
		sw		a5,32(sp)	
		sw		a6,36(sp)	
		addi	a0,sp,12				// argument 0 to mos_enter_call
		mv		a1,a7
		jal		mos_enter_ecall			// jump to mos_entry_point
		lw		a0,12(sp)
		lw		a1,16(sp)	
		lw		a2,20(sp)	
		lw		a3,24(sp)	
		lw		a4,28(sp)	
		lw		a5,32(sp)	
		lw		a6,36(sp)	
		
		// we now need to return to the caller with all registers intact
		//TODO: this uses t0 and trashes it - need to speak with Hoglet about this and maybe look at change API
		lw		ra,4(sp)				// caller ra
		lw		t0,8(sp)				// caller pc
		lw		sp,0(sp)				// caller sp
		addi	sp,sp,(10*4)
		jr		t0						// return

mos_irq:	
		//TODO: this is very much belt-and-braces, we could probably
		//rely on C properly preserving some of these registers


		//on entry:
		// q0 = address after ecall
		// q1 = interrupt status - we will cancel bit 2
		// q2 = callers ra
		// q3 = callers sp
		// x2,x2 already corrupted

		// if q3 isn't in MOS stack then set to MOS stack - allow MOS to reenter, maybe change this?
		picorv32_getq_insn(sp, q3)
		la		ra,mos_stack_base
		blt		sp,ra,.news2
		la		ra,mos_stack_top
		blt		sp,ra,.nonews2

		// we weren't in the mos set to MOS stack
.news2:	la		sp,mos_stack_top
.nonews2:

		// stack all remaining regs
		addi	sp,sp,-(4*29)
		sw x3,   0*4(sp)
		sw x4,   1*4(sp)
		sw x5,   2*4(sp)
		sw x6,   3*4(sp)
		sw x7,   4*4(sp)
		sw x8,   5*4(sp)
		sw x9,   6*4(sp)
		sw x10,  7*4(sp)
		sw x11,  8*4(sp)
		sw x12,  9*4(sp)
		sw x13, 10*4(sp)
		sw x14, 11*4(sp)
		sw x15, 12*4(sp)
		sw x16, 13*4(sp)
		sw x17, 14*4(sp)
		sw x18, 15*4(sp)
		sw x19, 16*4(sp)
		sw x20, 17*4(sp)
		sw x21, 18*4(sp)
		sw x22, 19*4(sp)
		sw x23, 20*4(sp)
		sw x24, 21*4(sp)
		sw x25, 22*4(sp)
		sw x26, 23*4(sp)
		sw x27, 24*4(sp)
		sw x28, 25*4(sp)
		sw x29, 26*4(sp)
		sw x30, 27*4(sp)
		sw x31, 28*4(sp)

		//TODO: jump through IRQ1V
		jal		mos_default_IRQ1V

		lw x3,   0*4(sp)
		lw x4,   1*4(sp)
		lw x5,   2*4(sp)
		lw x6,   3*4(sp)
		lw x7,   4*4(sp)
		lw x8,   5*4(sp)
		lw x9,   6*4(sp)
		lw x10,  7*4(sp)
		lw x11,  8*4(sp)
		lw x12,  9*4(sp)
		lw x13, 10*4(sp)
		lw x14, 11*4(sp)
		lw x15, 12*4(sp)
		lw x16, 13*4(sp)
		lw x17, 14*4(sp)
		lw x18, 15*4(sp)
		lw x19, 16*4(sp)
		lw x20, 17*4(sp)
		lw x21, 18*4(sp)
		lw x22, 19*4(sp)
		lw x23, 20*4(sp)
		lw x24, 21*4(sp)
		lw x25, 22*4(sp)
		lw x26, 23*4(sp)
		lw x27, 24*4(sp)
		lw x28, 25*4(sp)
		lw x29, 26*4(sp)
		lw x30, 27*4(sp)
		lw x31, 28*4(sp)

		//mask irq
		picorv32_getq_insn(ra, q1)
		andi	ra,ra,~0x10
		picorv32_setq_insn(q1, ra)

		picorv32_getq_insn(x1, q2)
		picorv32_getq_insn(x2, q3)

		picorv32_retirq_insn()


interrupts_handle:

		picorv32_setq_insn(q3, sp)	// save stack pointer

		// quick check for ecall - TODO: make this more efficient when we sort out interrupts		
		picorv32_getq_insn(x1, q1)
		andi	x2,x1,2
		beqz	x2,.notecall		// not an illegal instr
		picorv32_getq_insn(x1, q0)
		andi	x2,x1,1
		bnez	x2,.notecall		// not a full-fat instr
		//need to do half-word loads here because of misalign 
		lh		x2,-4(x1)
		xori	x2,x2,0x73
		bnez	x2,.notecall
		lh		x2,-2(x1)
		xori	x2,x2,0x0
		bnez	x2,.notecall
		j		mos_enter


.notecall:
		picorv32_getq_insn(x1, q1)
		andi	x2,x1,0x10
		bnez	x2, mos_irq

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

		jal deice_enter

		la x1, interrupts_regs

		lw x3,   2*4(x1)
		lw x4,   3*4(x1)
		lw x5,   4*4(x1)
		lw x6,   5*4(x1)
		lw x7,   6*4(x1)
		lw x8,   7*4(x1)
		lw x9,   8*4(x1)
		lw x10, 9*4(x1)
		lw x11, 10*4(x1)
		lw x12, 11*4(x1)
		lw x13, 12*4(x1)
		lw x14, 13*4(x1)
		lw x15, 14*4(x1)
		lw x16, 15*4(x1)
		lw x17, 16*4(x1)
		lw x18, 17*4(x1)
		lw x19, 18*4(x1)
		lw x20, 19*4(x1)
		lw x21, 20*4(x1)
		lw x22, 21*4(x1)
		lw x23, 22*4(x1)
		lw x24, 23*4(x1)
		lw x25, 24*4(x1)
		lw x26, 25*4(x1)
		lw x27, 26*4(x1)
		lw x28, 27*4(x1)
		lw x29, 28*4(x1)
		lw x30, 29*4(x1)
		lw x31, 30*4(x1)


		// get back ra, sp and stash in q2/q3
		lw x2,	 0*4(x1)
		picorv32_setq_insn(q2, x2)	// save return address
		lw x2,	 1*4(x1)
		picorv32_setq_insn(q3, x2)	// save stack pointer

		// q0 is set to return address of interrupt
		lw x2,   31*4(x1)
		picorv32_setq_insn(q0, x2)

		// q1 is set to pending interrupts
		lw x2,   32*4(x1)
		picorv32_setq_insn(q1, x2)

		// stashed above
		picorv32_getq_insn(x1, q2)
		picorv32_getq_insn(x2, q3)

		picorv32_retirq_insn()
