unsigned char * const SCREENBASE = (unsigned char * const)0xFFFF7C00;

char *scrptr;

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


extern void buserror(void);

void main(void) {
	scrptr = SCREENBASE;

	printstr("TEST1");

	asm("ebreak");
}
