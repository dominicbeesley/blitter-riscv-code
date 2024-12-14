/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.
*/

MEMORY {
	/* this runs from as a boot MOS at FFFFC000 */ 
	ROM : 		ORIGIN = 0xFFFFC000, LENGTH = 0x00004000
}

SECTIONS {
	.code : {
		. = 0x0000;
		start*(.text);
		*(.text);
		*(.text*);
		*(.rodata);
		*(.rodata*);
		*(.srodata);
		. = ALIGN(4);
	} > ROM

	.data : {
		. = ALIGN(4);
		_data_start = .;
		*(.data);	
		*(.data*);	
		*(.sdata);	
		*(.sdata*);	
		. = ALIGN(4);
	} >ROM

	.hwvects ORIGIN(ROM) + 0x3FF8 : {
		. = ALIGN(4);
		*(reset);		
		*(irq);
		. = ALIGN(4);
		_bob = .;
	} >ROM

	

}
