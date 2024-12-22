#include "debug_print.h"
#include "deice.h"

uint8_t * const SCREENBASE = (unsigned char *)0xFFFF7C00;
extern unsigned char const splash;

volatile uint8_t *scrptr;
char a = 'x';

void debug_print_init() {
	
}

void debug_print_ch(char c) {
	deice_print_char(c);
}

void debug_print_str(const char *s) {
	char c;
	do {
		c = *s++;
		if (c) debug_print_ch(c);		
	} while(c);
}

void debug_print_hex_nyb(uint8_t n) {
	n = n & 0xF;
	if (n <= 9)
		debug_print_ch('0'+n);
	else 
		debug_print_ch('A'+n-10);
}

void debug_print_hex_byte(uint8_t n) {
	debug_print_hex_nyb(n >> 4);
	debug_print_hex_nyb(n);
}

void debug_print_hex_word(uint32_t n) {
	debug_print_hex_byte((uint8_t)(n >> 24));
	debug_print_hex_byte((uint8_t)(n >> 16));
	debug_print_hex_byte((uint8_t)(n >> 8));
	debug_print_hex_byte((uint8_t)(n));
}

void debug_print_hex_halfword(uint16_t n) {
	debug_print_hex_byte((uint8_t)(n >> 8));
	debug_print_hex_byte((uint8_t)(n));
}
