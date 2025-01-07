		.globl mos_stack_base
		.globl mos_stack_top
		.globl mos_stack_len

// MOS Stack - full descending


		.bss
		.align(4)
		.globl mos_stack_base
		.globl mos_stack_top
mos_stack_base:
		.space (0x1000)
mos_stack_top:
mos_stack_len = mos_stack_top-mos_stack_base
		.text
