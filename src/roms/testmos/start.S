		#include "custom_ops.S"


sheila			= 0xFFFFFE00

start:		// disable all but timer, illins, bus interrupts
		li	t0,0xFFFFFFF8
		picorv32_maskirq_insn(t0, t0)

		la 	sp,0x10000		// setup set stack in ChipRAM at 00010000 growing downwards

		// copy r/w data to RAM from ROM
		//ASSERT data_size & 3 == 0
		la	t0,_data_load_start
		la	t1,_data_start
		la	t2,_data_size
		beq	t2,x0,.cskip		// no data to copy
.clp:		lw	t3,0(t0)
		sw	t3,0(t1)
		addi	t0,t0,4
		addi	t1,t1,4
		addi	t2,t2,-4
		bgt	t2,zero,.clp
.cskip:
		
		//init BSS
		//ASSERT bss_size & 3 == 0
		la	t0,_bss_start
		la	t2,_bss_size
.blp:		sw	zero,0(t0)
		addi	t0,t0,4
		addi	t2,t2,-4
		bgt	t2,zero,.blp

		jal	deice_init

		jal	mos_reset

		//application
		jal	init
		j	main

	.globl buserror
buserror:	mv	s0, ra
		li	a1, 0x12
		lw	a0, 0(a1)
		jal	hexword
		mv	ra, s0
		ret



		.section reset, "x", @progbits
reset_hw:	j start
		.section irq, "x", @progbits
irq_hw:		j interrupts_handle


