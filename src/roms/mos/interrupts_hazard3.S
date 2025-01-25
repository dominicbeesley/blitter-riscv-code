		#include "custom_ops_hazard3.h"
		#include "interrupts.h"
		#include "deice.h"

		.globl interrupts_disable
		.globl interrupts_handle
		.globl interrupts_regs
		.globl interrupts_init


#define SAVEX(rn)	\
		sw	rn, deice_REG_OFF(rn)(sp)
#define LOADX(rn)	\
		lw	rn, deice_REG_OFF(rn)(sp)
		
		.bss
		.globl user_error_pend
		.globl user_stack_frame
//This is used in mos_enter_ecall to flag up exceptions
		.align(2)
user_stack_frame:
		.word	0
user_error_pend:
		.word	0

		
		.section rodata
str_unex:	.asciz "An unexpected exception: "

		.text
		.align(2)

interrupts_disable:
		// this was started as a disable on picorv32, for now retain that behaviour but TODO: change interface to enable?

		// ensure bus, instr, timer
		andi	a0, a0, INT_ALL
		xori	a0, a0, INT_ALL
		slli	a0, a0, 16			// shift into WINDOW part of register

		csrrw	a0, hazard3_csr_meiea, a0
		srli	a0, a0, 16			// shift back and return old value for restore
		xori	a0, a0, INT_ALL

		ret


interrupts_init:
		// set up irq vectors stuff
		//csrrs	a0, misa, x0		// TODO: detect whether using external interrupt controller
		// for now assume external interrupt controller present

		//TODO: vector to at least separate out soft/external/etc interrupts but that requires some space
		la	a0, trap_table+1		//vectored through table in shims area
		csrrw	x0, mtvec, a0

		// set up the H3 custom irq controller
		li	t0, (0xFF0<<16)|(0)
		csrrw	x0, hazard3_csr_meipra, t0	// make irq low priority, debug and nmi high (assume only 1 bit of priority)
		
		li	t0, (0 << 16)|(0)
		csrrw	x0, hazard3_csr_meiea, t0	// disable all external interrupts

		li	t0, 1<<11
		csrrs	x0, mie, t0			// enable external interrupts
		csrrsi	x0, mstatus, 1<<3		// enable global interrupts

		ret




		//TODO: move this to own file
handle_ex_11_ecall:
		// we should be entered with
		// sp set up to MOS stack pointing at a deice frame
		// with the following registers saved in frame: a0, a1, sp, mepc, meicontext, mstatus, mcause

//CHECK TO SEE IF WE ENTERED OS AT THIS POINT AND SET A PER-PROCEsS/USER ENV pointer
//TODO: port to pico
//TODO: detect stack swap earlier
//TODO: proper user environment
		lw	a1, deice_REG_OFF(sp)(sp)		// get back caller stack pointer
		la	a0, mos_stack_base
		blt	a1, a0, tt_newstack
		la	a0, mos_stack_top
		blt	a1, a0, tt_okstack
tt_newstack:
		// the stack was swapped entering the OS so setup pointers
		//TODO: proper user process structure (handlers?)
		la	a1, user_stack_frame
		sw	sp, 0(a1)
		la	a1, user_error_pend
		sw	zero, 0(a1)	

tt_okstack:	

		//TODO: we can dispense with most of this - only a0-a7 need to be restored?
		// save the arguments and return address register, trust GCC to trash Tx and save Sx
		SAVEX(ra)
		SAVEX(a2)
		SAVEX(a3)
		SAVEX(a4)
		SAVEX(a5)
		SAVEX(a6)
		SAVEX(a7)
		SAVEX(t0)
		SAVEX(t1)
		SAVEX(t2)
		SAVEX(t3)
		SAVEX(t4)
		SAVEX(t5)
		SAVEX(t6)

		SAVEX(s0)
		SAVEX(s1)
		SAVEX(s2)
		SAVEX(s3)
		SAVEX(s4)
		SAVEX(s5)
		SAVEX(s6)
		SAVEX(s7)
		SAVEX(s8)
		SAVEX(s9)
		SAVEX(s10)
		SAVEX(s11)
		SAVEX(gp)
		SAVEX(tp)


		
		addi		a0,sp,deice_REG_OFF(a0)		// point at arguments as first param
		mv		a1,a7				// ecall number
		
		// exit interrupts and enter mos via mos_enter_ecall
		la		ra, handle_ex_11_ecall_exit
		la		a2, mos_enter_ecall
		csrw		mepc, a2
		mret
		
handle_ex_11_ecall_exit:
		
		nop
		nop

		LOADX(ra)
		LOADX(a0)
		LOADX(a1)
		LOADX(a2)
		LOADX(a3)
		LOADX(a4)
		LOADX(a5)
		LOADX(a6)
		LOADX(a7)
		LOADX(t0)	//TODO: use scratch register to preserve
		LOADX(t1)
		LOADX(t2)
		LOADX(t3)
		LOADX(t4)
		LOADX(t5)
		LOADX(t6)

		LOADX(s0)
		LOADX(s1)
		LOADX(s2)
		LOADX(s3)
		LOADX(s4)
		LOADX(s5)
		LOADX(s6)
		LOADX(s7)
		LOADX(s8)
		LOADX(s9)
		LOADX(s10)
		LOADX(s11)
		LOADX(gp)
		LOADX(tp)

		la	t0, user_stack_frame
		lw	t0, 0(t0)
		bne	sp, t0, not_user_exit
		
		// we are about to exit back to user process, check for pending errors //TODO: and other upcalls?
		la	t0, user_error_pend
		lw	t0, 0(t0)
		beqz	t0, not_user_exit

		// error pending call into user's error handler
		mv	a0, t0	// error pointer
		la	t0, active_handlers
		lw	t0, 16(t0)	// error handler

		LOADX(sp)
		jr	t0					// return to caller

not_user_exit:
		lw	t0, deice_REG_OFF(mepc)(sp)		// get return address from stack
		addi	t0, t0, 4				// ecall is always 32 bit

		LOADX(sp)	
		
		jr	t0					// return to caller
		

handle_breakpoint:
		// save other caller save regs in ABI TODO: check out __attribute__ interrupt and tweak...
		SAVEX(ra)
		SAVEX(t0)
		SAVEX(t1)
		SAVEX(t2)
		SAVEX(t3)
		SAVEX(t4)
		SAVEX(t5)
		SAVEX(t6)
		SAVEX(a2)
		SAVEX(a3)
		SAVEX(a4)
		SAVEX(a5)
		SAVEX(a6)
		SAVEX(a7)



handle_debug:
		//we're going to enter deice so we need to get out of the interrupt handler somehow but keep
		//all the interrupts turned off...

		//save rest of registers in deice frame		
		SAVEX(gp)
		SAVEX(tp)
		SAVEX(s0)
		SAVEX(s1)
		SAVEX(s2)
		SAVEX(s3)
		SAVEX(s4)
		SAVEX(s5)
		SAVEX(s6)
		SAVEX(s7)
		SAVEX(s8)
		SAVEX(s9)
		SAVEX(s10)
		SAVEX(s11)
		
		mv	a0, sp		// parameter to deice_enter_hazard3
		
		jal	deice_enter_hazard3

		//TODO: just set PC here - should maybe tweak interrupt regs?
		lw	a0, deice_REG_OFF(mepc)(sp)
		csrw	mepc, a0

		LOADX(ra)
		LOADX(t0)
		LOADX(t1)
		LOADX(t2)
		LOADX(t3)
		LOADX(t4)
		LOADX(t5)
		LOADX(t6)
		LOADX(a0)
		LOADX(a1)
		LOADX(a2)
		LOADX(a3)
		LOADX(a4)
		LOADX(a5)
		LOADX(a6)
		LOADX(a7)
		LOADX(gp)
		LOADX(tp)
		LOADX(s0)
		LOADX(s1)
		LOADX(s2)
		LOADX(s3)
		LOADX(s4)
		LOADX(s5)
		LOADX(s6)
		LOADX(s7)
		LOADX(s8)
		LOADX(s9)
		LOADX(s10)
		LOADX(s11)
		LOADX(sp)

		mret


trap_unex:	// an unexpected exception occurred just dump to deice log - don't worry about
		// actually entering deice yet?
		//	caller's sp,ra,a1,a0 stacked 
		la	a0, str_unex
		jal	debug_print_str

		csrr	a0, mcause
		jal	debug_print_hex_word

		li	a0, '\n'
		jal	debug_print_ch
		
		li	s0, 1
		mv	s1, sp
		li	s2, 35
tu_lp:		mv	a0, s0
		jal	debug_print_hex_byte
		li	a0, ':'
		jal	debug_print_ch
		lw	a0, 0(s1)
		jal	debug_print_hex_word
		li	a0, '\n'
		jal	debug_print_ch
		addi	s1,s1,4
		addi	s0,s0,1
		bne	s0,s2,tu_lp


lock:		j	lock




//////////////////////////////////////// SHIMS AREA /////////////////////////////////////////////

//TODO: all the interrupts / traps can be moved to main code area using mtvec and dispense with
//the trampolines

		.macro VEC dest
s\@:		j	\dest
		.align(2)
		.endm


		// this must be placed in top-half of ROM with the vectors
		// to ensure it's mapped in to memory at FF FF00 at boot time
		// and will jump "down" to correct memory 7D 0000 when running
		.section entry_shims, "ax", @progbits
		// hopefully this is at the start of the section and already aligned
		// it needs to be aligned to the nearest power of two beyond the vector
		// table's end
		.align(6)			
trap_table:	VEC(trap_exception_s)	// 0 - internal interrupt
		VEC(trap_unex_s)	// 1
		VEC(trap_unex_s)	// 2
		VEC(trap_unex_s)	// 3 - software interrupt
		VEC(trap_unex_s)	// 4
		VEC(trap_unex_s)	// 5
		VEC(trap_unex_s)	// 6
		VEC(trap_unex_s)	// 7 - timer interrupt
		VEC(trap_unex_s)	// 8
		VEC(trap_unex_s)	// 9
		VEC(trap_unex_s)	// 10
		VEC(trap_external_s)	// 11 - external interrupt


		// an unexpected trap occurred
trap_unex_s:	// fall through an check mcause when stack set up

trap_exception_s:
		csrrw	x0, mscratch, a0	// save caller's a0
		// we're gonna need a stack...check what current sp is
		// if it's not in mos_stack area make it so
		la	a0, mos_stack_base
		blt	sp, a0, t_newstack
		la	a0, mos_stack_top
		blt	sp, a0, t_okstack
t_newstack:	mv	a0, sp			// save old stack pointer
		la	sp, mos_stack_top	// set up stack - TODO: this should be stored somewhere when re-entering upcalls/event handlers
		j	t_oks2
t_okstack:	mv	a0, sp
t_oks2:		// we now have a stack 
		addi	sp, sp, -4*DEICE_INTERRUPT_REGISTERS	// set up DeIce stack frame, but only save minimum of regs for latency

		// save registers we have already stowed
		sw	a0, deice_REG_OFF(sp)(sp)// caller's sp
		csrr	a0, mscratch		
		sw	a0, deice_REG_OFF(a0)(sp)// caller's a0

		// save csrs possibly needed at exit of handler		
		csrr	a0, mepc
		sw	a0, deice_REG_OFF(mepc)(sp)// caller's ra
  		// Set bit 1 when reading to clear+save mie.mtie and mie.msie
  		csrrsi 	a0, hazard3_csr_meicontext, 0x2
  		sw 	a0, deice_REG_OFF(meicontext)(sp)
  		csrr 	a0, mstatus
  		sw 	a0, deice_REG_OFF(mstatus)(sp)

  		// save mcause - might be wanted by DeIce
  		csrr 	a0, mcause
  		sw 	a0, deice_REG_OFF(mcause)(sp)

		// save working registers in dispatch stuff
  		SAVEX(a1)

		csrr	a0, mcause
		bltz	a0, t_unx
		// use table to enter relevant routine in non-shims area
		slli	a0, a0, 2
		la	a1, tbl_except
		add	a0, a1, a0
		lw	a0, 0(a0)
		jr	a0			// jump to table entry

t_unx:		// mcause top bit set - we must have dropped through from trap_unex_s
		la	ra, trap_unex
		jr	ra			// unex trap in main code

		.align(2)
tbl_except:	.word	trap_unex		// handle_ex_0_inst_misalign
		.word	trap_unex		// handle_ex_1_inst_access_fault
		.word	trap_unex		// handle_ex_2_inst_illegal
		.word	handle_breakpoint	// handle_ex_3_inst_breakpoint
		.word	trap_unex		// handle_ex_4_load_misalign
		.word	trap_unex		// handle_ex_5_load_access_fault
		.word	trap_unex		// handle_ex_6_store_misalign
		.word	trap_unex		// handle_ex_7_store_access_fault
		.word	trap_unex		// 8
		.word	trap_unex		// 9
		.word	trap_unex		// 10
		.word	handle_ex_11_ecall	// 11

///////////////////////////// external interrupt handler
trap_external_s:
		csrrw	x0, mscratch, a0	// save caller's a0
		// we're gonna need a stack...check what current sp is
		// if it's not in mos_stack area make it so
		la	a0, mos_stack_base
		blt	sp, a0, et_newstack
		la	a0, mos_stack_top
		blt	sp, a0, et_okstack
et_newstack:	mv	a0, sp			// save old stack pointer
		la	sp, mos_stack_top	// set up stack - TODO: this should be stored somewhere when re-entering upcalls/event handlers
		j	et_oks2
et_okstack:	mv	a0, sp
et_oks2:	// we now have a stack 
		addi	sp, sp, -4*DEICE_INTERRUPT_REGISTERS	// set up DeIce stack frame, but only save minimum of regs for latency

		// save registers we have already stowed
		sw	a0, deice_REG_OFF(sp)(sp)// caller's sp
		csrr	a0, mscratch		
		sw	a0, deice_REG_OFF(a0)(sp)// caller's a0

		// save csrs needed at exit of handler		
		csrr	a0, mepc
		sw	a0, deice_REG_OFF(mepc)(sp)// caller's ra
  		// Set bit 1 when reading to clear+save mie.mtie and mie.msie
  		csrrsi 	a0, hazard3_csr_meicontext, 0x2
  		sw 	a0, deice_REG_OFF(meicontext)(sp)
  		csrr 	a0, mstatus
  		sw 	a0, deice_REG_OFF(mstatus)(sp)
  		csrr	a0, mcause
  		sw	a0, deice_REG_OFF(mcause)(sp)

		// save working registers in dispatch stuff
  		SAVEX(a1)

		// save other caller save regs in ABI TODO: fewer for NMI?, TODO: check out __attribute__ interrupt and tweak...
		SAVEX(ra)
		SAVEX(t0)
		SAVEX(t1)
		SAVEX(t2)
		SAVEX(t3)
		SAVEX(t4)
		SAVEX(t5)
		SAVEX(t6)
		SAVEX(a2)
		SAVEX(a3)
		SAVEX(a4)
		SAVEX(a5)
		SAVEX(a6)
		SAVEX(a7)


  		j 	get_next_irq
dispatch_irq:
  		// Preemption priority was configured by meinext update, so enable preemption:
  		csrsi 	mstatus, 0x8
  		// meinext is pre-shifted by 2, so only an add is required to index table
  		la 	a1, external_irq_table
  		add 	a1, a1, a0
  		jalr 	ra, a1
  		// Disable IRQs on returning so we can sample the next IRQ
  		csrci 	mstatus, 0x8
get_next_irq:
  		// Sample the current highest-priority active IRQ (left-shifted by 2) from
  		// meinext, and write 1 to the LSB to tell hardware to tell hw to update
  		// meicontext with the preemption priority (and IRQ number) of this IRQ
  		csrrsi 	a0, hazard3_csr_meinext, 0x1
  		// MSB will be set if there is no active IRQ at the current priority level
  		bgez 	a0, dispatch_irq
no_more_irqs:
  		// Restore saved context and return from handler
  		lw 	a0, deice_REG_OFF(mepc)(sp)
  		csrw 	mepc, a0
  		lw 	a0, deice_REG_OFF(meicontext)(sp)
  		csrw 	hazard3_csr_meicontext, a0
  		lw 	a0, deice_REG_OFF(mstatus)(sp)
  		csrw 	mstatus, a0
  		lw	ra, deice_REG_OFF(ra)(sp)
  		lw 	a0, deice_REG_OFF(a0)(sp)

		// restore working registers in dispatch stuff
  		LOADX(a1)

		// restore other caller save regs in ABI TODO: fewer for NMI?
		LOADX(ra)
		LOADX(t0)
		LOADX(t1)
		LOADX(t2)
		LOADX(t3)
		LOADX(t4)
		LOADX(t5)
		LOADX(t6)
		LOADX(a2)
		LOADX(a3)
		LOADX(a4)
		LOADX(a5)
		LOADX(a6)
		LOADX(a7)


  		lw 	sp, deice_REG_OFF(sp)(sp)		// we must do this last of course
  		mret

		.align(4)
external_irq_table:
		VEC(mos_default_IRQ1V)
		VEC(handle_debug)
		VEC(mos_default_NMI)




		.section irq, "ax", @progbits
irq_hw:		j 	trap_unex_s		// treat an early interrupt as an unexpected event
