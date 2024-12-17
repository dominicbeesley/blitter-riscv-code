#include "interrupts.h"
#include "deice.h"
#include "moslib.h"
#include "mos.h"
#include "hardware.h"
#include <stddef.h>
#include "debug_print.h"

/*
void irq_handle(void) {
	for (int i = 1; i < 36; i++) {
		DEBUG_PRINT_STR("       X");
		DEBUG_PRINT_HEX_BYTE(i);
		DEBUG_PRINT_STR(": ");
		DEBUG_PRINT_HEX_WORD(interrupts_regs[i-1]);
	}

	deice_enter();

	do { } while (1);
}
*/


void init(void) {

	DEBUG_PRINT_INIT;

}


void vdu_str(const char *str) {
	if (!str)
		return;
	char c;
	while ((c = *str++)) {
		mos_oswrch(c);
	}
}


void main(void) {

	vdu_str("BOO!");

	do {

		int c = mos_osrdch();
		if (c < 0)
		{
			DEBUG_PRINT_STR("ESCAPE!");
			//TODO: use OSBYTE 126
			mos_osbyte(126, NULL, NULL, NULL);
		}
		else
		{
			if (c < ' ' || c >= 0x7F)
				DEBUG_PRINT_HEX_BYTE(c);
			else
				mos_oswrch(c);
		}

	} while (1);

}


