		.global splash

		.section .rodata
		.align 4
splash:
	#ifdef FLAVOUR_PICORV32
		.incbin "splash_picorv32.mo7"
	#endif
	#ifdef FLAVOUR_HAZARD3
		.incbin "splash_hazard3.mo7"
	#endif
