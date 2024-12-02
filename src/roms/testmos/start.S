

start:

		la 	sp,0x10000		// setup set stack in ChipRAM at 00010000
		
		j	main

//		la	t0, splash		// source
//		la	t1, 0xFFFF7C00		// dest
//		li	t2, 0xFFFF8000		// dest end
//
//		//copy loop
//lp:
//		lw	t3,0(t0)
//		sw	t3,0(t1)
//		addi	t0,t0,4
//		addi	t1,t1,4
//		bne	t1,t2,lp
//
//HERE:	j HERE



		.section reset, "x", @progbits
reset_hw:
j		start
		.section irq, "x", @progbits
irq_hw:
beq		x0,x0,irq_hw


