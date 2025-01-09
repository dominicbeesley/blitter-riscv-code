
#ifdef FLAVOUR_PICORV32
		#include "custom_ops_picorv32.h"
#endif
#ifdef FLAVOUR_HAZARD3
		#include "custom_ops_hazard3.h"
#endif


sheila			= 0xFFFFFE00

		.align(2)
start:	
#ifdef FLAVOUR_PICORV32
		// disable all but timer, illins, bus interrupts
		li	t0,0xFFFFFFF8
		picorv32_maskirq_insn(t0, t0)
#endif

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

		// this must be placed in top-half of ROM with the vectors
		// to ensure it's mapped in to memory at FF FF00 at boot time
		// and will jump "down" to correct memory 7D 0000 when running
	.section entry_shims, "ax", @progbits

		// TODO: this is the only way I've found to get the linker to tell us
		// the absolute address....li doesn't work and la is PC relative which
		// is no good for this purpose!
start_addr:	.word start

reset_handle_enter:
		// need to do a long/absolute jump here as CPU will think we're running
		// in FFFFCxxx when we're actually in 007D0xxx
		lw	t0,start_addr
		jr	t0

	.section reset, "ax", @progbits
reset_hw:	j reset_handle_enter


