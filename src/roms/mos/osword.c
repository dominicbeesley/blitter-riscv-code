#include "osword.h"
#include "mos.h"
#include "buffers.h"
#include "mos_shared.h"
#include "interrupts.h"
#include <stddef.h>
#include "debug_print.h"

void osword_default_WORDV(uint8_t A, void *block) {

	if (A == 0) {
		//TODO: HOGLET'S API - change to be beeb compatible somehow?
		uint8_t *ptr = (uint8_t *)(((uint32_t *)block)[0]);
		uint32_t size = (((uint32_t *)block)[1]);
		uint32_t min = (((uint32_t *)block)[2]);
		uint32_t max = (((uint32_t *)block)[3]);
		int ix = 0;
		do {
			int c = RDCHV();
			if (c < 0) {
				((uint32_t *)block)[1] = -1;
				return;
			} else if (c == 0x7F && ix > 0) {
				ix--;
				WRCHV(8);
				WRCHV(' ');
				WRCHV(8);
			} else if (c >= min && c <= max && ix < size-1) {
				ptr[ix++] = c;
				WRCHV(c);
			} else if (c == 13) {
				WRCHV(13);
				WRCHV(10);
				((uint32_t *)block)[1] = ix;					
				ptr[ix++] = 13;
				return;
			} else {
				WRCHV(7);
			}

		} while (1);
	} else if (A == 1) {
		//read time
		volatile uint8_t *p = TIME_VAL1_MSB + OSB_TIME_SWITCH - 1;
		volatile uint8_t *q = (uint8_t *)block;
		for (int i = 0 ; i < 5; i++) {
			*q++=*p--;	// reverse endian
		}
		return;
	} else if (A == 2) {
		//write time
		volatile uint8_t *p = TIME_VAL1_MSB + (OSB_TIME_SWITCH ^ 0xF) - 1;
		volatile uint8_t *q = (uint8_t *)block;
		for (int i = 0 ; i < 5; i++) {
			*p--=*q++;	// reverse endian
		}
		return;
	}

	DEBUG_PRINT_STR("UNKNOWN OSWORD:");
	DEBUG_PRINT_HEX_BYTE(A);
	DEBUG_PRINT_CH('\n');
}