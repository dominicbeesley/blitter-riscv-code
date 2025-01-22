/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.
*/

SECTIONS {
	.code : {
		start*(.text);
		*(.text);
		*(.text*);
		*(.rodata);
		*(.rodata*);
		*(.srodata);
		*(.got);
		*(.got*);
		. = ALIGN(4);
		_cend = .;

		. = ALIGN(4);
		_data_start = .;
		*(.data);	
		*(.data*);	
		*(.sdata);	
		*(.sdata*);	
		. = ALIGN(4);

	}

	.bss (NOLOAD): {
		. = ALIGN(4);
		_bss_start = .;
		*(.bss);
		*(.memory);
		*(.sbss);
		. = ALIGN(4);
	} 
	_bss_size = SIZEOF(.bss);

	

}
