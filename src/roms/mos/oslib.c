#include "oslib.h"

// quick wrappers for calling OS ecalls - adapted from Hoglet's BBC BASIC port
//TODO: fix up clobbers
//TODO: move to a separate library

#define OSCLOBBERS "ra", "t0", "t1", "t2", "t3", "t4", "t5", "t6"

void oslib_ossysctrl(int a) {
   register int a0 asm ("a0") = a;
   register int a7 asm ("a7") = OS_SYS_CTRL;
   asm volatile ("ecall"
                 : // outputs
                 : // inputs
                   "r"  (a0),
                   "r"  (a7)
                 : OSCLOBBERS
                 );
}

// This call never returns
void oslib_osquit(int a) {
   register int a0 asm ("a0") = a;
   register int a7 asm ("a7") = OS_QUIT;
   asm volatile ("ecall"
                 : // outputs
                 : // inputs
                   "r"  (a0),
                   "r"  (a7)
                 : OSCLOBBERS
                 );
}

void oslib_oshandlers(int num, void *handler_fn, void *handler_data, void **old_handler_fn, void **old_handler_data) {
   register int   a0 asm ("a0") = num;
   register void *a1 asm ("a1") = handler_fn;
   register void *a2 asm ("a2") = handler_data;
   register int   a7 asm ("a7") = OS_HANDLERS;
   asm volatile ("ecall"
                 : // outputs
                   "+r" (a1),
                   "+r" (a2)
                 : // inputs
                   "r"  (a0),
                   "r"  (a1),
                   "r"  (a2),
                   "r"  (a7)
                 : OSCLOBBERS
                 );
   if (old_handler_fn) {
      *old_handler_fn = a1;
   }
   if (old_handler_data) {
      *old_handler_data = a2;
   }
}


void oslib_oswrch(unsigned char vdu) {   // Write to display or other output stream (VDU)
   register int a0 asm ("a0") = vdu;
   register int a7 asm ("a7") = OS_WRCH;
   asm volatile ("ecall"
                 : // outputs

                 : // inputs
                   "r"  (a0),
                   "r"  (a7)
                 : OSCLOBBERS
                 );
}

int oslib_osrdch(void) { // Get character from console input
   register int a0 asm ("a0");
   register int a7 asm ("a7") = OS_RDCH;
   asm volatile ("ecall"
                 : // outputs
                   "+r" (a0)
                 : // inputs
                   "r"  (a7)
                 : OSCLOBBERS
                 );
   return a0;
}

void oslib_osbyte(int al, int *x, int *y, int *c) {
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
                 : OSCLOBBERS
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

int oslib_osword(int al, void *xy) {
   register int a0 asm ("a0") = al;
   register int *a1 asm ("a1") = xy;
   register int a2 asm ("a2") = 0;
   register int a7 asm ("a7") = OS_WORD;
   asm volatile ("ecall"
                 : // outputs
                   "+r" (a2)
                 : // inputs
                   "r"  (a0),
                   "r"  (a1),
                   "r"  (a7)
                 : // clobber
                   "memory", OSCLOBBERS
                 );
   return a2;
}

unsigned long oslib_osfile(int reason, char *filename, uint32_t *load, uint32_t *exec, uint32_t *start, uint32_t *end) {
   register int      a0 asm ("a0") = reason;
   register char    *a1 asm ("a1") = filename;
   register uint32_t a2 asm ("a2") = load  ? (*load ) : 0;
   register uint32_t a3 asm ("a3") = exec  ? (*exec ) : 0;
   register uint32_t a4 asm ("a4") = start ? (*start) : 0;
   register uint32_t a5 asm ("a5") = end   ? (*end  ) : 0;
   register int   a7 asm ("a7") = OS_FILE;
   asm volatile ("ecall"
                 : // outputs
                   "+r"  (a0),
                   "+r"  (a2),
                   "+r"  (a3),
                   "+r"  (a4),
                   "+r"  (a5)
                 : // inputs
                   "r"  (a0),
                   "r"  (a1),
                   "r"  (a2),
                   "r"  (a3),
                   "r"  (a4),
                   "r"  (a5),
                   "r"  (a7)
                 : OSCLOBBERS                   
                 );
   if (load) {
      *load = a2;
   }
   if (exec) {
      *exec = a3;
   }
   if (start) {
      *start = a4;
   }
   if (end) {
      *end = a5;
   }
   return a0;
}

void * oslib_osfind(int a, char *p) {
   register int   a0 asm ("a0") = a;
   register void *a1 asm ("a1") = p;
   register int   a7 asm ("a7") = OS_FIND;
   asm volatile ("ecall"
                 : // outputs
                   "+r" (a0)
                 : // inputs
                   "r"  (a0),
                   "r"  (a1),
                   "r"  (a7)
                 : OSCLOBBERS                   
                 );
   return (void *) a0;
}

int oslib_osbget(void *chan, int *peof) { // Read a byte from a file
   register int   a0 asm ("a0");
   register void *a1 asm ("a1") = chan;
   register int   a7 asm ("a7") = OS_BGET;
   asm volatile ("ecall"
                 : // outputs
                   "+r" (a0)
                 : // inputs
                   "r"  (a1),
                   "r"  (a7)
                 : OSCLOBBERS                   
                 );
   if (peof) {
      if (a0 < 0) {
         *peof = -1; // TRUE
      } else {
         *peof = 0; // FALSE
      }
   }
   return a0 & 0xff;
}

void oslib_osbput(void *chan, unsigned char byte) { // Write a byte to a file
   register int   a0 asm ("a0") = byte;
   register void *a1 asm ("a1") = chan;
   register int   a7 asm ("a7") = OS_BPUT;
   asm volatile ("ecall"
                 : // outputs
                 : // inputs
                   "r"  (a0),
                   "r"  (a1),
                   "r"  (a7)
                 : OSCLOBBERS
                 );
}


int oslib_osargs(int reason, void *chan, void *data) {
   register int           a0 asm ("a0") = reason;
   register void         *a1 asm ("a1") = chan;
   register uint32_t      a2 asm ("a2") = *(uint32_t *)data;
   register int           a7 asm ("a7") = OS_ARGS;
   asm volatile ("ecall"
                 : // outputs
                   "+r" (a0),
                   "+r" (a2)
                 : // inputs
                   "r"  (a0),
                   "r"  (a1),
                   "r"  (a2),
                   "r"  (a7)
                 : OSCLOBBERS
                 );
   if (data) {
      *(uint32_t *)data = a2;
   }
   return a0;
}

int oslib_osgbpb(uint8_t reason, uint8_t chan, uint32_t *data, uint32_t *num, uint32_t *ptr, int *peof) {
   register uint32_t a0 asm ("a0") = reason;
   register uint32_t a1 asm ("a1") = chan;
   register uint32_t a2 asm ("a2") = data ? (*data) : 0;
   register uint32_t a3 asm ("a3") = num  ? (*num ) : 0;
   register uint32_t a4 asm ("a4") = ptr  ? (*ptr ) : 0;
   register int   a7 asm ("a7") = OS_GBPB;
   asm volatile ("ecall"
                 : // outputs
                   "+r" (a0),
                   "+r" (a2),
                   "+r" (a3),
                   "+r" (a4)
                 : // inputs
                   "r"  (a0),
                   "r"  (a1),
                   "r"  (a2),
                   "r"  (a3),
                   "r"  (a4),
                   "r"  (a7)
                 : OSCLOBBERS
                 );
   if (data) {
      *data = a2;
   }
   if (num) {
      *num = a3;
   }
   if (ptr) {
      *ptr = a4;
   }
   *peof = (a0 & 0x80000000) ? 1 : 0;
   return a0 & 0xff;
}

void oslib_oscli(char *cmd) {      // Execute an emulated OS command
   register int a0 asm ("a0") = (int) cmd;
   register int a7 asm ("a7") = OS_CLI;
   asm volatile ("ecall"
                 : // outputs
                   "+r" (a0)
                 : // inputs
                   "r"  (a0),
                   "r"  (a7)
                 : OSCLOBBERS
                 );
}


void oslib_read_unsigned(uint32_t baseflags, const char **str, uint32_t *num) {
	register uint32_t a0 asm ("a0") = baseflags;
	register const char * a1 asm ("a1") = *str;
	register uint32_t a2 asm ("a2") = *num;

	register int a7 asm ("a7") = OS_READUNS;
   	asm volatile ("ecall"
                 : // outputs
                   "+r" (a0),
                   "+r" (a1),
                   "+r" (a2)
                 : // inputs
                   "r"  (a0),
                   "r"  (a1),
                   "r"  (a2),
                   "r"  (a7)
                 : OSCLOBBERS
                 );
	*str = a1;
	*num = a2;
}