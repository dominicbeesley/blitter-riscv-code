
unsigned char * const SCREENBASE = (unsigned char * const)0xFFFF7C00;
extern unsigned char const splash;


void main(void) {
	int * p = (int *)&splash;
	int * q = (int *)SCREENBASE;
	for (int i = 0; i < 0x100; i++) {
		q[i] = p[i];
	}

	do { } while (1);
}

//
//		la	t0, splash		// source
//		la	t1, 0xFFFF7C00		// dest
//		li	t2, 0xFFFF8000		// dest end
//
//		//copy loop
//lp:
//		lw	t3,0(t0)
//		sw	t3,0(t1)
//		addi	t0,t0,4
//		addi	t1,t1,4
//		bne	t1,t2,lp
//
//HERE:	j HERE
