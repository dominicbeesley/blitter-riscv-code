#include "osword.h"
#include "mos.h"
#include "buffers.h"
#include "mos_shared.h"
#include "interrupts.h"
#include <stddef.h>
#include "debug_print.h"

void osword_default_WORDV(uint8_t A, void *block) {

	if (A == 1) {
		//read time
		volatile uint8_t *p = TIME_VAL1_MSB + OSB_TIME_SWITCH - 1;
		volatile uint8_t *q = (uint8_t *)block;
		for (int i = 0 ; i < 5; i++) {
			*q++=*p--;	// reverse endian
		}
		return;
	}

	DEBUG_PRINT_STR("UNKNOWN OSWORD:");
	DEBUG_PRINT_HEX_BYTE(A);
	DEBUG_PRINT_CH('\n');
}