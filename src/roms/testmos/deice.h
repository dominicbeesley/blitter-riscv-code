#ifndef __DEICE_H__
#define __DEICE_H__

#include <stdint.h>

/* defines for UART TODO: move to hardware include?*/
#define ACIA_DATA (*((volatile uint8_t*)0xFFFFFE09))
#define ACIA_STAT (*((volatile uint8_t*)0xFFFFFE08))
#define SER_ULA   (*((volatile uint8_t*)0xFFFFFE10))

#define ACIA_STAT_RDRF 0x01
#define ACIA_STAT_TDRE 0x02

#define FN_GET_STAT			0xFF    // reply with device info
#define FN_READ_MEM			0xFE    // reply with data
#define FN_WRITE_M			0xFD    // reply with status (+/-)
#define FN_READ_RG			0xFC    // reply with registers
#define FN_WRITE_RG			0xFB    // reply with status
#define FN_RUN_TARG			0xFA    // reply (delayed) with registers
#define FN_SET_BYTES		0xF9    // reply with data (truncate if error)
#define FN_IN				0xF8    // input from port
#define FN_OUT				0xF7    // output to port
#define FN_MIN				0xF7    // MINIMUM RECOGNIZED FUNCTION CODE
#define FN_ERROR			0xF0    // error reply to unknown op-code


extern void deice_init(void);
extern void deice_print_char(char c);
extern void deice_print_str(const char *str);
extern void deice_enter(void);

#endif