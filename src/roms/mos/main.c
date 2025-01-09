#include "interrupts.h"
#include "deice.h"
#include "moslib.h"
#include "mos.h"
#include "hardware.h"
#include <stddef.h>
#include "debug_print.h"
#include "oslib.h"
#include <string.h>

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


void cmd_go(const char *tail) {
	vdu_str("GOGOGOGO\n\r");
}

typedef void (*CMD_FN)(const char *tail);


struct t_cmd_ent {
	const char *name;
	CMD_FN cmd;
};

const struct t_cmd_ent cmd_tbl[] = {
	{ "GO", cmd_go }
};
#define N_CMD (sizeof(cmd_tbl) / (sizeof(const char *)))


int cmdcmp(const char *name, const char *match, int l) {
	const char *p = name;
	const char *q = match;

	while (l && *p && *q && *p == *q) {
		p++;
		q++;
		l--;
	}

	if (l == 0 && *p == 0 )
		return 0;

	if (l == 1 && *q == '.')
		return 0;

	return -1;
}

void main(void) {

	vdu_str("Risc V MOS - Dossy 2024\r\n");

	char buf[256];

	while (1) {
		WRCHV('*');
		struct OSWORD_0 osw0 = {
			buf,
			256,
			' ',
			0x7F
		};

		WORDV(0, &osw0);
		buf[osw0.size] = 0;		

		if (buf[0] >= ' ') {

			

			char *p = buf;
			while (*p && *p <= ' ') p++;

			//TODO: call OSCLI

			int l = 0;
			char *q = p;
			while (*q > ' ') {
				q++;
				l++;
			}

			while (*q && *q <= ' ') q++;

			if (l) {

				CMD_FN c = NULL;

				//TODO: .

				for (int i = 0; i < N_CMD; i++) {
					if (!cmdcmp(cmd_tbl[i].name, p, l)) {
						c = cmd_tbl[i].cmd;
					}
				}
				
				if (c) {
					c(q);
				} else {
					vdu_str("Unrecognized command\r\n");
				}
		}


		}

	}

}


