#include "interrupts.h"
#include "deice.h"
#include "moslib.h"
#include "mos.h"
#include "hardware.h"
#include <stddef.h>

uint8_t * const SCREENBASE = (unsigned char *)0xFFFF7C00;
extern unsigned char const splash;

volatile uint8_t *scrptr;
char a = 'x';


void printch(char c) {
	*scrptr++=c;
	if (scrptr >= SCREENBASE + 25*40) {
		scrptr = SCREENBASE;
	}
}

void printstr(const char *s) {
	char c;
	do {
		c = *s++;
		if (c) printch(c);		
	} while(c);
}

void hexnyb(unsigned int n) {
	n = n & 0xF;
	if (n <= 9)
		printch('0'+n);
	else 
		printch('A'+n-10);
}

void hexbyte(unsigned int n) {
	hexnyb(n >> 4);
	hexnyb(n);
}

void hexword(unsigned int n) {
	hexbyte(n >> 24);
	hexbyte(n >> 16);
	hexbyte(n >> 8);
	hexbyte(n);
}

/*
void irq_handle(void) {
	for (int i = 1; i < 36; i++) {
		printstr("       X");
		hexbyte(i);
		printstr(": ");
		hexword(interrupts_regs[i-1]);
	}

	deice_enter();

	do { } while (1);
}
*/


void init(void) {

	scrptr = SCREENBASE + 40*10;

}

extern void buserror(void);

void main(void) {
	int * p = (int *)&splash;
	int * q = (int *)SCREENBASE;


	for (int i = 0; i < 0x100; i++) {
		q[i] = p[i];
	}

	printstr("BOO!");

	do {
		uint32_t t = ((uint32_t)TIME_VAL1_MSB[1] << 24)
		| ((uint32_t)TIME_VAL1_MSB[2] << 16)
		| ((uint32_t)TIME_VAL1_MSB[3] << 8)
		| ((uint32_t)TIME_VAL1_MSB[4] << 0);

		interrupts_disable(0xFF);
		volatile uint8_t *tmp = scrptr;
		scrptr = SCREENBASE;
		hexword(t);
		hexbyte(OSB_TIME_SWITCH);

		printch(' ');
		hexbyte(OSB_KEY_SEM);
		printch(' ');
		hexbyte(sheila_SYSVIA_ier);
		printch(' ');
		hexbyte(KEYNUM_FIRST);
		printch(' ');
		hexbyte(KEYNUM_LAST);
		printch(' ');
		hexbyte(KEY_ROLLOVER_1);
		printch(' ');
		hexbyte(KEY_ROLLOVER_2);
		printch(' ');
		hexbyte(OSB_KEY_STATUS);
		printch(' ');
		hexbyte(ESCAPE_FLAG);


		scrptr = tmp;
		interrupts_disable(0x00);

		int c = mos_osrdch();
		if (c < 0)
		{
			printstr("ESCAPE!");
			//TODO: use OSBYTE 126
			mos_osbyte(126, NULL, NULL, NULL);
		}
		else
			if (c <= ' ' || c >= 0x7F)
				hexbyte(c);
			else
				mos_oswrch(c);

	} while (1);

}


