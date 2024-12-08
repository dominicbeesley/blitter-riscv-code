
start:		
		
		//init BSS
		//ASSERT bss_size & 3 == 0
		la	t0,_bss_start
		la	t2,_bss_size
.blp:		sw	zero,0(t0)
		addi	t0,t0,4
		addi	t2,t2,-4
		bgt	t2,zero,.blp

		
		j	main



