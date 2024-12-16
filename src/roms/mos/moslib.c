#include "moslib.h"
#include "mos.h"

void mos_oswrch(unsigned char vdu) {   // Write to display or other output stream (VDU)
	 register int a0 asm ("a0") = vdu;
	 register int a7 asm ("a7") = OS_WRCH;
	 asm volatile ("ecall"
								 : // outputs

								 : // inputs
									 "r"  (a0),
									 "r"  (a7)
								 );
}


int mos_osrdch(void) { // Get character from console input
   register int a0 asm ("a0");
   register int a7 asm ("a7") = OS_RDCH;
   asm volatile ("ecall"
                 : // outputs
                   "+r" (a0)
                 : // inputs
                   "r"  (a7)
                 );
   return a0;
}


//TODO: int types here?
void mos_osbyte(int al, int *x, int *y, int *c) {
   register int a0 asm ("a0") = al;
   register int a1 asm ("a1") = x ? (*x) : 0;
   register int a2 asm ("a2") = y ? (*y) : 0;
   register int a3 asm ("a3");
   register int a7 asm ("a7") = OS_BYTE;
   asm volatile ("ecall"
                 : // outputs
                   "+r" (a1),
                   "+r" (a2),
                   "+r" (a3)
                 : // inputs
                   "r"  (a0),
                   "r"  (a1),
                   "r"  (a2),
                   "r"  (a7)
                 );
   if (x) {
      *x = a1;
   }
   if (y) {
      *y = a2;
   }
   if (c) {
      *c = a3;
   }
}
