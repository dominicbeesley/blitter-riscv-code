#include <stdint.h>


extern void hexbyte(unsigned int n);
extern void printstr(const char *s);

/* defines for UART */

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



int deice_put_byte(uint8_t b);
int deice_get_byte(void);
void deice_print_char(char c);
void deice_print_str(const char *str);
uint8_t deice_checksum(void);

void deice_fn_get_stat(void);
void deice_fn_read_mem(void);
void deice_fn_write_m(void);
void deice_fn_read_rg(void);
void deice_fn_write_rg(void);
void deice_fn_run_targ(void);
void deice_fn_set_bytes(void);
void deice_fn_in(void);
void deice_fn_out(void);
void deice_fn_error(void);

#define COMSZ 0xF0

const uint8_t TSTG[] = {
	0x35,	// risc-v 32 bit
	COMSZ,	// combuf size
	0x0,	// no tasking
	0x00, 0x00, 0xFF, 0xFF, // paging range
	0x2,	// bp reg length
	0x02, 0x90, // breakpoint instruction (use compressed form)
	'r', 'i', 's', 'c', '-', 'v', ' ', 'm', 'o', 'n', 'i', 't', 'o', 'r', ' ', 'v', '0', '.', '1', '-', 'B', 'B', 'C',
	0x0
};

uint8_t combuf [COMSZ];

void deice_init(void) {

		// set up BBC's ACIA for RTS/CTS at 19200 baud - later use OS routines when available
		SER_ULA = 0x40;			// 19200,19200
		ACIA_STAT = 0b01010111; // master reset
		ACIA_STAT = 0b01010110; // RTS high, no interrupts, 8N1, div64
		
		deice_print_str("Hello\n");

		//deice_run_flag=0
}

int deice_put_byte(uint8_t b) {
	int timeout = 0x10000;

	while (timeout) {
		if (ACIA_STAT & ACIA_STAT_TDRE) {
			ACIA_DATA = b;
			return 0;
		}
		else
			timeout--;
	}
	return -1;
}

int deice_get_byte(void) {
	int timeout = 0x10000;

	while (timeout) {
		if (ACIA_STAT & ACIA_STAT_RDRF) {
			uint8_t r = ACIA_DATA;
			hexbyte(r);
			return r;
		}
		else
			timeout--;
	}
	return -1;
}



void deice_print_char(char c) {
	deice_put_byte((uint8_t)c & 0x7F);
}

void deice_print_str(const char *str) {
	while (str && *str) {
		deice_print_char(*str++);
	}
}


void deice_main(void) {

restart:

	printstr("MAIN");

	uint8_t *p = combuf;

	int i = deice_get_byte();
	if (i < 0) goto restart;

	*p++ = (uint8_t)i;

	i = deice_get_byte();
	if (i < 0 || i > COMSZ) goto restart;
	uint8_t n = (uint8_t)i;
	*p++ = n;

	while (n > 0) {
		i = deice_get_byte();
		if (i < 0) goto restart;		
		*p++ = (uint8_t)n;
	}

	// get checksum
	i = deice_get_byte();
	if (i < 0) goto restart;		
	uint8_t cs_s = (uint8_t)i;

	uint8_t cs_c = deice_checksum();

	if ((cs_s + cs_c) & 0xFF) goto restart;

	switch (combuf[0]) {
		case	FN_GET_STAT:	deice_fn_get_stat();break;
		case	FN_READ_MEM:	deice_fn_read_mem();break;
		case	FN_WRITE_M:		deice_fn_write_m();break;
		case	FN_READ_RG:		deice_fn_read_rg();break;
		case	FN_WRITE_RG:	deice_fn_write_rg();break;
		case	FN_RUN_TARG:	deice_fn_run_targ();break;
		case	FN_SET_BYTES:	deice_fn_set_bytes();break;
		case	FN_IN:			deice_fn_in();break;
		case	FN_OUT:			deice_fn_out();break;
		default:
			deice_fn_error();break;
	}

	goto restart;

}

void deice_send(void) {
	uint8_t cs = deice_checksum();
	uint8_t n = combuf[1];
	combuf[n+2] = 0-cs;
	n = n + 3;
	uint8_t *p = combuf;

	while (n) {
		if (deice_put_byte(*p++) < 0)
			return;
		n--;
	}
}

void deice_fn_get_stat(void) {
	uint8_t *p = combuf+1;
	const uint8_t *q = TSTG;
	uint8_t n = sizeof(TSTG);
	*p++ = n;
	while (n) {
		*p++ = *q++;
		n--;
	}
	deice_send();
}

void deice_fn_read_mem(void) {

}

void deice_fn_write_m(void) {

}

void deice_fn_read_rg(void) {

}

void deice_fn_write_rg(void) {

}

void deice_fn_run_targ(void) {

}

void deice_fn_set_bytes(void) {

}

void deice_fn_in(void) {

}

void deice_fn_out(void) {

}

void deice_send_status(uint8_t code) {
	combuf[0] = code;
	combuf[1] = 1;
	deice_send();
}

void deice_fn_error(void) {
	deice_send_status(FN_ERROR);
}

uint8_t deice_checksum(void) {
	uint8_t n = combuf[1] + 2;
	uint8_t r = 0;
	uint8_t *p = combuf;
	while (n) {
		r += *p++;
		n--;
	}
	return r;
}