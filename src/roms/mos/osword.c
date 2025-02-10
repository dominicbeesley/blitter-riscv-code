#include "osword.h"
#include "mos.h"
#include "buffers.h"
#include "mos_shared.h"
#include "interrupts.h"
#include <stddef.h>
#include "debug_print.h"


//returns -1 for Carry TODO: API: Discuss with Hoglet
int8_t osword_default_WORDV(uint8_t A, void *block) {

	if (A == 0) {
		//TODO: HOGLET'S API - change to be beeb compatible somehow?
		uint8_t *ptr = (uint8_t *)(((uint32_t *)block)[0]);
		uint32_t maxindex = (((uint32_t *)block)[1]);
		uint32_t min = (((uint32_t *)block)[2]);
		uint32_t max = (((uint32_t *)block)[3]);
		int ix = 0;
		do {
			int c = RDCHV();
			if (c < 0) {
				((uint32_t *)block)[1] = -1;
				return -1;
			} else if (OSB_VDU_QSIZE != 0 && (OSB_OUT_STREAM & 2) == 0) {
				// output to VDU drivers to complete multibyte sequence
				WRCHV(c);	 
			} else if (c == 0x7F) {
				if (ix > 0) {
					ix--;
					WRCHV(0x7F);
				}
			} else if (c == 21) {
				//ctrl-u - delete line
				while (ix > 0)
				{
					WRCHV(0x7F);
					ix--;
				}
			} else {
				ptr[ix] = c;
				if (c == 13) {
					//end of line
					WRCHV(13);
					WRCHV(10);
					((uint32_t *)block)[1] = ix;
					return 0;					
				} else if (ix >= maxindex) {
					WRCHV(7);
				} else {
					WRCHV(c);
					if (c >= min && c <= max) {
						ix++;
					}
				}
			}

		} while (1);
	} else if (A == 1) {
		//read time
		volatile uint8_t *p = TIME_VAL1_MSB + OSB_TIME_SWITCH - 1;
		volatile uint8_t *q = (uint8_t *)block;
		for (int i = 0 ; i < 5; i++) {
			*q++=*p--;	// reverse endian
		}
		return 0;
	} else if (A == 2) {
		//write time
		volatile uint8_t *p = TIME_VAL1_MSB + (OSB_TIME_SWITCH ^ 0xF) - 1;
		volatile uint8_t *q = (uint8_t *)block;
		for (int i = 0 ; i < 5; i++) {
			*p--=*q++;	// reverse endian
		}
		return 0;
	}

	DEBUG_PRINT_STR("UNKNOWN OSWORD:");
	DEBUG_PRINT_HEX_BYTE(A);
	DEBUG_PRINT_CH('\n');
	return -1;
}