		.globl deice_stack_base
		.globl deice_stack_top
		.globl deice_stack_len

// MOS Stack - full descending


		.bss
		.align(4)
		.globl deice_stack_base
		.globl deice_stack_top
deice_stack_base:
		.space (0x1000)
deice_stack_top:
deice_stack_len = deice_stack_top-deice_stack_base
		.text
