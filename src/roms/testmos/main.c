#include "interrupts.h"
#include "deice.h"
#include "moslib.h"

unsigned char * const SCREENBASE = (unsigned char * const)0xFFFF7C00;
extern unsigned char const splash;

char *scrptr;
char a = 'x';


void printch(char c) {
	*scrptr++=c;
	if (((int)scrptr) & 0x8000) {
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

	scrptr = SCREENBASE;

	interrupts_disable(0);

}

extern void buserror(void);

void main(void) {
	int * p = (int *)&splash;
	int * q = (int *)SCREENBASE;


	for (int i = 0; i < 0x100; i++) {
		q[i] = p[i];
	}

	printstr("BOO!");

	mos_oswrch('A');	

	// force a bus error (if we can)
	//buserror();

//	asm ("c.ebreak"); // enter debugger


	do { } while (1);
}


