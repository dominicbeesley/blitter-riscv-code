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
