OUTPUT_ARCH( "riscv" )
OUTPUT_FORMAT("elf32-littleriscv")
ENTRY( _start )
SECTIONS
{
  /* text: code section */
  . = 0x00100000;

  .text : { 
    start*(.text);
    *(.text);
    *(.text*);
    *(.rodata);
    *(.rodata*);
    *(.srodata);
  }

  .data : {
    . = ALIGN(4);
    _data_start = .;
    *(.data); 
    *(.data*);  
    *(.sdata);  
    *(.sdata*); 
    . = ALIGN(4);
  } 
  _data_size = SIZEOF(.data);
  _data_load_start = LOADADDR(.data);

  .bss (NOLOAD): {
    . = ALIGN(4);
    _bss_start = .;
    *(.bss);
    *(.bss*);
    *(.memory);
    *(.sbss);
    *(.sbss*);
    . = ALIGN(4);
    _bss_end = .;
  }


  /* gnu_build_id: readonly build identifier */
  .gnu_build_id : { *(.note.gnu.build-id) }

}
