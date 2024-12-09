#include "deice.h"
#include "interrupts.h"

//TODO: REMOVE THESE DEBUGGING
#undef DEICE_DEBUG 
#ifdef DEICE_DEBUG
extern void hexbyte(unsigned int n);
extern void printstr(const char *s);
extern void hexword(unsigned int n);
#endif


/* local forward declarations */
//TODO: move get/put byte to a hardware specific file, rest is non-specific
static int deice_put_byte(uint8_t b);
static int deice_get_byte(void);
static uint8_t deice_checksum(void);
static void deice_fn_get_stat(void);
static void deice_fn_read_mem(void);
static void deice_fn_write_m(void);
static void deice_fn_read_rg(uint8_t fn);
static void deice_fn_write_rg(void);
static void deice_fn_set_bytes(void);
static void deice_fn_in(void);
static void deice_fn_out(void);
static void deice_fn_error(void);
static void deice_send_status(uint8_t);

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
		
#ifdef DEICE_DEBUG
		deice_print_str("Hello\n");
#endif

		//deice_run_flag=0
}

int deice_put_byte(uint8_t b) {
	int timeout = 0x10000;

	while (timeout) {
		if (ACIA_STAT & ACIA_STAT_TDRE) {
			ACIA_DATA = b;
#ifdef DEICE_DEBUG			
			printstr(">");
			hexbyte(b);
#endif
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
#ifdef DEICE_DEBUG
			printstr("<");
			hexbyte(r);
#endif
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


void deice_enter(void) {
	//TODO: not sure this is the right place for this but we need
	//to readjust the PC pointer depending on the type of interrupt
	//adjust back to previous instruction if illegal op / break / bus error
	uint32_t irq_type = interrupts_regs[32];
	if (irq_type & 0x6) {
		if (interrupts_regs[31] & 1) {
			//16 bit instruction
			interrupts_regs[31]-=2;
		} else {
			interrupts_regs[31]-=4;
		}
	}
	

	//we expect the process state to be saved in interrupts_regs
	deice_fn_read_rg(FN_RUN_TARG);
}

void deice_main(void) {

restart:

#ifdef DEICE_DEBUG
	printstr("MAIN");
#endif

	uint8_t *p = combuf;

	int i = deice_get_byte();
	if (i < FN_MIN) goto restart;
	*p++ = (uint8_t)i;

#ifdef DEICE_DEBUG
	printstr("!F!");
#endif

	i = deice_get_byte();
	if (i < 0 || i > COMSZ) goto restart;
	uint8_t n = (uint8_t)i;
	*p++ = n;

#ifdef DEICE_DEBUG
	printstr("!N!");
#endif

	while (n > 0) {
		i = deice_get_byte();
		if (i < 0) goto restart;		
		*p++ = (uint8_t)i;
		n--;
	}

#ifdef DEICE_DEBUG
	printstr("!P!");
#endif

	// get checksum
	i = deice_get_byte();
	if (i < 0) goto restart;		
	uint8_t cs_s = (uint8_t)i;
	uint8_t cs_c = deice_checksum();

#ifdef DEICE_DEBUG
	printstr("!CS!");
	hexbyte(cs_s);
	hexbyte(cs_c);
#endif

	if ((cs_s + cs_c) & 0xFF) goto restart;

#ifdef DEICE_DEBUG
	printstr("!EXE!");
#endif

	switch (combuf[0]) {
		case	FN_GET_STAT:	deice_fn_get_stat();break;
		case	FN_READ_MEM:	deice_fn_read_mem();break;
		case	FN_WRITE_M:		deice_fn_write_m();break;
		case	FN_READ_RG:		deice_fn_read_rg(FN_READ_RG);break;
		case	FN_WRITE_RG:	deice_fn_write_rg();break;
		case	FN_RUN_TARG:	return; // exit deice back to interrupt return sequence
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

// read big-endian 32 bit address
uint8_t *deice_read_addr(uint8_t **p) {
	return (uint8_t *)(
		  (*(*p)++ << 24) 
		| (*(*p)++ << 16) 
		| (*(*p)++ << 8) 
		| (*(*p)++ << 0)
		);
}

void deice_fn_read_mem(void) {
	uint8_t *p = combuf+2;
	uint8_t *addr = deice_read_addr(&p);	// get address
	uint8_t n = *p;							// get length

#ifdef DEICE_DEBUG
	printstr("READMEM");
	hexword((unsigned int)addr);
	printstr(",");
	hexbyte(n);
#endif

	combuf[1] = n;							// return length
	p = combuf+2;
	while (n) {
		*p++=*addr++;
		n--;
	}
	deice_send();
}

void deice_fn_write_m(void) {
	uint8_t *p = combuf+1;
	int n = (int)*p++;						// get length
	uint8_t *addr = deice_read_addr(&p);	// get address
	n -= 4;

#ifdef DEICE_DEBUG
	printstr("WRITEMEM");
	hexword((unsigned int)addr);
	printstr(",");
	hexbyte(n);
#endif

	uint8_t *src = combuf + 6;
	uint8_t *dest = addr;
	int c = n;
	while (c > 0) {
		*dest++=*src++;
		c--;
	}

	//verify
	src = combuf + 6;
	dest = addr;
	c = n;
	while (c > 0) {
		if (*dest++ != *src++)
			deice_send_status(1);	//error - mismatch
		c--;
	}

	deice_send_status(0);			// worked!

}

void deice_fn_read_rg(uint8_t fn) {
	combuf[0] = fn;
	combuf[1] = (INTERRUPTS_REGS_N*4)+1;			// registers + target status

	//copy as bytes - alignment!
	uint8_t *p = (uint8_t *)combuf+2;
	uint8_t *q = (uint8_t *)interrupts_regs;
	int n = (INTERRUPTS_REGS_N*4);
	while (n) {
		*p++=*q++;
		n--;
	}
	// +1 byte for target status
	*p++ = 1;									
	deice_send();
	deice_main();
}

void deice_fn_write_rg(void) {
	if (combuf[1] != (INTERRUPTS_REGS_N*4)+1) 
		deice_send_status(1);	// error - wrong length
	uint8_t *p = (uint8_t *)combuf+2;
	uint8_t *q = (uint8_t *)interrupts_regs;
	int n = (INTERRUPTS_REGS_N*4);
	while (n) {
		*q++=*p++;
		n--;
	}
	//ignore target status?!
	deice_send_status(0); // success
}


void deice_fn_set_bytes(void) {

}

void deice_fn_in(void) {
	// TODO: we don't actually have ports - but instead do a read/write without verify
	deice_fn_error();
}

void deice_fn_out(void) {
	// TODO: we don't actually have ports - but instead do a read/write without verify
	deice_fn_error();	
}

void deice_send_status(uint8_t code) {
	combuf[2] = code;
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