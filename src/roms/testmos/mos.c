#include "mos.h"

//TODO:remove
extern void printstr(const char *s);
extern void printch(char c);
extern void hexword(unsigned int n);

void mos_enter_ecall(struct mos_args *args, uint32_t a7) {
	//asm("ebreak");
	//TODO: check for 0xAC0000?
	switch (a7 & 0xFF) {
		case OS_WRCH:
			printch(args->a0);
			return;
		case OS_NEWL:
			printstr("\n\r");
			return;

	}


	printstr(" A0:");
	hexword(args->a0);
	printstr(" A1:");
	hexword(args->a1);
	printstr(" A2:");
	hexword(args->a2);
	printstr(" A3:");
	hexword(args->a3);
	printstr(" A4:");
	hexword(args->a4);
	printstr(" A5:");
	hexword(args->a5);
	printstr(" A6:");
	hexword(args->a6);
	printstr(" A7:");
	hexword(a7);
}
