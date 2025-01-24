

#include "vdu_print.h"
#include "mos.h"

void vdu_str(const char *str) {
	if (!str)
		return;
	char c;
	while ((c = *str++)) {
		if (c == 13)
			WRCHV(10);
		WRCHV(c);
	}
}

void vdu_hexnyb(unsigned hex) {
	hex = hex & 0xF;
	if (hex >= 10) 
		WRCHV('a' + hex - 10);
	else
		WRCHV('0' + hex);
}

void vdu_hex8(unsigned hex) {
	vdu_hexnyb(hex >> 4);
	vdu_hexnyb(hex);
}

void vdu_hex32(unsigned hex) {
	vdu_hex8( hex >> 24);
	vdu_hex8( hex >> 16);
	vdu_hex8( hex >> 8);
	vdu_hex8( hex );
}
