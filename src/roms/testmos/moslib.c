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
