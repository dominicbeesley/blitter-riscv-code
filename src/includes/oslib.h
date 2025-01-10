#ifndef __OSLIB_H__
#define __OSLIB_H__

#include <stdint.h>

// RISC-V Co Processor Sys Call Numbers

//#define ECALL_BASE  0x00AC0000
#define ECALL_BASE  0x00000000

#define OS_QUIT     (ECALL_BASE +  0)
#define OS_CLI      (ECALL_BASE +  1)
#define OS_BYTE     (ECALL_BASE +  2)
#define OS_WORD     (ECALL_BASE +  3)
#define OS_WRCH     (ECALL_BASE +  4)
#define OS_NEWL     (ECALL_BASE +  5)   // Unused
#define OS_RDCH     (ECALL_BASE +  6)
#define OS_FILE     (ECALL_BASE +  7)
#define OS_ARGS     (ECALL_BASE +  8)
#define OS_BGET     (ECALL_BASE +  9)
#define OS_BPUT     (ECALL_BASE + 10)
#define OS_GBPB     (ECALL_BASE + 11)
#define OS_FIND     (ECALL_BASE + 12)
#define OS_SYS_CTRL (ECALL_BASE + 13)
#define OS_HANDLERS (ECALL_BASE + 14)
#define OS_ERROR    (ECALL_BASE + 15)   // Unused

// these are DB additions - TODO: reorder
#define OS_READUNS  (ECALL_BASE + 16)	// OS_ReadUnsigned



struct OSWORD_0 {
	char * buf;
	uint32_t size;
	uint32_t min;
	uint32_t max;
};

extern void oslib_ossysctrl(int a);
extern void oslib_osquit(int a);
extern void oslib_oshandlers(int num, void *handler_fn, void *handler_data, void **old_handler_fn, void **old_handler_data);
extern void oslib_oswrch(unsigned char vdu);
extern int oslib_osrdch(void);
extern void oslib_osbyte(int al, int *x, int *y, int *c);
extern int oslib_osword(int al, void *xy);
extern unsigned long oslib_osfile(int reason, char *filename, uint32_t *load, uint32_t *exec, uint32_t *start, uint32_t *end);
extern void * oslib_osfind(int a, char *p);
extern int oslib_osbget(void *chan, int *peof);
extern void oslib_osbput(void *chan, unsigned char byte);
extern int oslib_osargs(int reason, void *chan, void *data);
extern int oslib_osgbpb(uint8_t reason, uint8_t chan, uint32_t *data, uint32_t *num, uint32_t *ptr, int *peof);
extern void oslib_oscli(char *cmd);

extern void oslib_read_unsigned(uint32_t baseflags, const char **str, uint32_t *num);

#endif