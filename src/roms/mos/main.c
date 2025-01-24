#include "interrupts.h"
#include "deice.h"
#include "mos.h"
#include "hardware.h"
#include <stddef.h>
#include "debug_print.h"
#include "oslib.h"
#include <string.h>
#include "vdu_print.h"

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

void hex2(uint32_t n) {
	if (n >= 0x10)
		hex2(n >> 4);
	n = n & 0xF;
	if (n >= 10)
		WRCHV('A'-10 + n);
	else
		WRCHV('0' + n);
}

void hex(uint32_t n) {
	if (n == 0)
		WRCHV('0');
	else
		hex2(n);
}

void cmd_go(const char *tail) {

	uint32_t n;
	oslib_read_unsigned(16, &tail, &n); 

	//TODO: properly start a new execution context

	register uint32_t a0 asm ("a0") = n;

	asm	volatile ("la sp, 0x10000"); // new stack
	asm volatile ("jr a0" : : "r" (a0));

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

const mos_error err_ESCAPE = { 0x27, "ESCAPE" };

void supervisor_prompt() {

	char buf[256];

	while (1) {
		WRCHV('*');
		struct OSWORD_0 osw0 = {
			buf,
			256,
			' ',
			0x7F
		};

		if (WORDV(0, &osw0) < 0) {
			vdu_str("ESCAPE FLAG");
			vdu_hex8(ESCAPE_FLAG);
			if (ESCAPE_FLAG & 0x80) {
				uint8_t X;
				uint8_t Y;
				BYTEV(126, &X, &Y);
			}
			vdu_str("\r");
			oslib_oserror(&err_ESCAPE);
		} else {
			buf[osw0.size] = 0;		

			if (buf[0] >= ' ') {

				

				char *p = buf;
				while (*p && (*p <= ' ' || *p == '*')) p++;

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
						vdu_str("Unrecognized command\r");
					}
				}
			}
		}
	}
}


void main(void) {

	vdu_str("Risc V MOS - Dossy 2024\r");

	supervisor_prompt();

}

