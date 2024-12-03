
#define INT_NMI		0x08
#define INT_IRQ		0x10
#define INT_DEBUG	0x10

extern void enable_interrupts(int mask);

unsigned char * const SCREENBASE = (unsigned char * const)0xFFFF7C00;
extern unsigned char const splash;

int irq_regs[32+4];						// 32 caller regs + q0..3

char *scrptr;
char a = 'x';


void printch(char c) {
	*scrptr++=c;
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

void irq_handle(void) {
	for (int i = 0; i < 36; i++) {
		printstr("       X");
		hexbyte(i);
		printstr(": ");
		hexword(irq_regs[i]);
	}

	do { } while (1);
}


void init(void) {
		scrptr = SCREENBASE;
}

void main(void) {
	int * p = (int *)&splash;
	int * q = (int *)SCREENBASE;


	for (int i = 0; i < 0x100; i++) {
		q[i] = p[i];
	}

	enable_interrupts(INT_DEBUG|INT_NMI);

	printstr("BOO!");

	do { } while (1);
}
