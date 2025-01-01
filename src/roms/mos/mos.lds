/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.
*/

MEMORY {
	/* this runs from BB (mk2)/ChipRam (mk3) 16K at
	7D 0000 - 7D 3FFF. This will enter reset in  MOSRAM at 
	FFFFCxxx which we just use for the vectors */ 
	ROM : 		ORIGIN = 0x007CC000, LENGTH = 0x00008000
	RAM : 		ORIGIN = 0x00008000, LENGTH = 0x00004000
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
		_cend = .;
	} > ROM

	.data : {
		. = ALIGN(4);
		_data_start = .;
		*(.data);	
		*(.data*);	
		*(.sdata);	
		*(.sdata*);	
		. = ALIGN(4);
	} >RAM AT> ROM
	_data_size = SIZEOF(.data);
    _data_load_start = LOADADDR(.data);

	.shims ORIGIN(ROM) + 0x7F00 : {
		. = ALIGN(4);
		*(entry_shims);
		_bob2 = .;
	} >ROM

	.hwvects ORIGIN(ROM) + 0x7FF8 : {
		. = ALIGN(4);
		*(reset);		
		*(irq);
		. = ALIGN(4);
		_bob = .;
	} >ROM

	.bss (NOLOAD): {
		. = ALIGN(4);
		_bss_start = .;
		*(.bss);
		*(.memory);
		*(.sbss);
		. = ALIGN(4);
	} > RAM
	_bss_size = SIZEOF(.bss);

	

}
