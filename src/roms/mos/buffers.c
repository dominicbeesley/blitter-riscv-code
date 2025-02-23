#include "buffers.h"
#include "mos_shared.h"
#include "mos.h"
#include "string.h"
#include "events.h"
#include "interrupts.h"
#include "handlers.h"
#include "osbyte.h"

#include <stddef.h>


volatile uint8_t  * const BUF_STARTS [] = {
	(volatile uint8_t *)0x03E0,
	(volatile uint8_t *)0x0A00,
	(volatile uint8_t *)0x0900,
	(volatile uint8_t *)0x0880,
	(volatile uint8_t *)0x0840,
	(volatile uint8_t *)0x0850,
	(volatile uint8_t *)0x0860,
	(volatile uint8_t *)0x0870,
	(volatile uint8_t *)0x08C0
};

uint32_t const BUF_LENGTHS [] = {
	0x20,
	0x100,
	0xC0,
	0x40,
	0x10,
	0x10,
	0x10,
	0x10,
	0x40
};

int buf_ix_in [BUFNOMAX+1];
int buf_ix_out [BUFNOMAX+1];

void buffers_init(void) {
	memset(buf_ix_in, 0, sizeof(int) * (BUFNOMAX + 1));
	memset(buf_ix_out, 0, sizeof(int) * (BUFNOMAX + 1));
}

// _OSBYTE_153 returns FLAG_C for 
uint8_t buffers_insert_check_ESC(uint8_t buffer_num, uint8_t data) {
	if (buffer_num == 0 || (buffer_num == 1 && OSB_RS423_MODE == 0)) {
			if (data == OSB_ESCAPE && OSB_ESC_ACTION == 0) {
				if (!(OSB_ESC_BRK & 0x01)) {
					if (event_raise(EVENT_06_ESCAPE, 0, data) & FLAG_C) {
						//not handled - set escape flag
						uint8_t X, Y;
						BYTEV(OSBYTE_125_ESCAPE_SET, &X, &Y);
					}
				}
				return 0;
			}
			event_raise(EVENT_02_CHAR_ENTER_BUFFER, 0, data);
	}			
	return buffers_insert(buffer_num, data);
}

// _OSBYTE_138
uint8_t buffers_insert(uint8_t buffer_num, uint8_t data) {
	return INSV(buffer_num, data);
}



uint8_t buffers_default_INSV(uint8_t buffer_num, uint8_t data) {

	if (buffer_num > BUFNOMAX)
		return FLAG_C;				// not our buffer return SEC

	uint32_t irq = interrupts_disable(INT_IRQ);

	int offso = buf_ix_in[buffer_num];
	int offs = offso;
	offs++;
	if (offs >= BUF_LENGTHS[buffer_num])
		offs = 0;

	if (offs == buf_ix_out[buffer_num]) {
		if (buffer_num < 2) 
			event_raise(EVENT_01_INPUT_BUFFER_FULL, buffer_num, 0);		
		interrupts_disable(irq);
		return FLAG_C;
	}
			
	BUF_STARTS[buffer_num][offso] = data;
	buf_ix_in[buffer_num] = offs;

	interrupts_disable(irq);
	return 0;
}

uint8_t buffers_default_REMV(uint8_t buffer_num, uint8_t flags, uint8_t *c) {

	if (buffer_num > BUFNOMAX)
		return FLAG_C;				// not our buffer return SEC

	uint8_t irq = interrupts_disable(INT_IRQ);
	uint8_t offso = buf_ix_out[buffer_num];
	uint8_t offsi = buf_ix_in[buffer_num];
	if (buf_ix_in[buffer_num] == offso)
	{
		interrupts_disable(irq);
		return FLAG_C;
	}

	*c = BUF_STARTS[buffer_num][offso];


	if (!(flags & FLAG_V))
	{
		//only advance if no peek
		offso++;
		if (offso >= BUF_LENGTHS[buffer_num])
			offso = 0;
		buf_ix_out[buffer_num] = offso;
		if (buffer_num >= 2 && offso == offsi)
			event_raise(EVENT_00_OUTPUT_BUFFER_EMPTY, buffer_num, 0);
	}

	interrupts_disable(irq);
	return 0;
}

int buffers_default_RDCHV(void) {
	do {
		//TODO: EXEC
		if (ESCAPE_FLAG & FLAG_N)
		{
			return -1;
		}
		//TODO: econet redirect here?
		//TODO: soft key expand here?
		//TODO: rs423 redirect
		uint8_t r;
		if (!(REMV(OSB_IN_STREAM, 0, &r) & FLAG_C))
			return (int)r;
	} while (1);
}


void buffers_flush(uint8_t flags, uint8_t buffer_num) {
	//TODO: SOUND / BUSY
	uint32_t irq = interrupts_disable(INT_IRQ);
	if (buffer_num <= 1) {
		OSB_SOFT_KEYLEN = 0;
		OSB_VDU_QSIZE = 0;
	}

	CNPV(buffer_num, FLAG_V);

	interrupts_disable(irq);
}


uint8_t buffers_default_CNPV(uint8_t buffer, uint8_t flags) {
	if (buffer > BUFNOMAX)
		return 0;
	if (flags & FLAG_V)
	{
		//PURGE
		buf_ix_out[buffer] = buf_ix_in[buffer];
		return 0;
	} else {
		int r = 0;
		uint32_t irq = interrupts_disable(INT_IRQ);

		r = buf_ix_in[buffer] - buf_ix_out[buffer];
		if (r < 0) {
			r = BUF_LENGTHS[buffer] + r;
		}

		if (flags & FLAG_C)
			r = BUF_LENGTHS[buffer] - r;

		interrupts_disable(irq);
		return r;
	}
}