#ifndef __MOS_H__
#define __MOS_H__

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


struct mos_args {
	uint32_t	a0;
	uint32_t	a1;
	uint32_t	a2;
	uint32_t	a3;
	uint32_t	a4;
	uint32_t	a5;
	uint32_t	a6;
};

#endif