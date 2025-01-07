#include "mos.h"
#include "mos_shared.h"
#include "hardware.h"
#include "interrupts.h"
#include "keyboard.h"
#include "buffers.h"
#include "osbyte.h"
#include "osword.h"
#include <stddef.h>
#include "debug_print.h"
#include "vdu.h"
#include <string.h>

INSV_FN INSV;
REMV_FN REMV;
RDCHV_FN RDCHV;
BYTEV_FN BYTEV;
WORDV_FN WORDV;
CNPV_FN CNPV;
WRCHV_FN WRCHV;

//Timer 1 100Hz period
#define T1PER (10000-2)

void mos_latch_write(uint8_t val) {
	//delay for at least 1us to ensure latch clocks
	DELAY1
	sheila_SYSVIA_orb = val;
}

void mos_enter_ecall(struct mos_args *args, uint32_t a7) {
	//asm("ebreak");
	//TODO: check for 0xAC0000?
	switch (a7 & 0xFF) {
		case OS_WRCH:
			WRCHV(args->a0);
			return;
		case OS_NEWL:
			DEBUG_PRINT_STR("\n\r");
			return;
		case OS_RDCH:
			args->a0 = RDCHV();
			return;
		case OS_BYTE:
			uint8_t r = BYTEV(args->a0, (uint8_t *)&args->a1, (uint8_t *)&args->a2);
			args->a3 = r;
			return;
		case OS_WORD:
			if (args->a0 == 0) {
				//TODO: HOGLET'S API - change to be beeb compatible somehow?
				uint8_t *ptr = (uint8_t *)(((uint32_t *)args->a1)[0]);
				int ix = 0;
				do {
					int c = RDCHV();
					if (c < 0) {
						args->a2 = -1;
						return;
					} else if (c == 0x7F) {
						if (ix > 0) {
							ix--;
							WRCHV(8);
							WRCHV(' ');
							WRCHV(8);
						}
					} else if (c >= ' ' && c < 0x7F) {
						ptr[ix++] = c;
						WRCHV(c);
					} else if (c == 13) {
						WRCHV(13);
						WRCHV(10);
						ptr[ix++] = 13;
						args->a2 = ix;					
						return;
					}

				} while (1);
			} else {
				WORDV(args->a0, (void *)args->a1);			
				return;
			}
	}

	DEBUG_PRINT_STR("ECALL\n");
	DEBUG_PRINT_STR(" A0:");
	DEBUG_PRINT_HEX_WORD(args->a0);
	DEBUG_PRINT_STR("\n A1:");
	DEBUG_PRINT_HEX_WORD(args->a1);
	DEBUG_PRINT_STR("\n A2:");
	DEBUG_PRINT_HEX_WORD(args->a2);
	DEBUG_PRINT_STR("\n A3:");
	DEBUG_PRINT_HEX_WORD(args->a3);
	DEBUG_PRINT_STR("\n A4:");
	DEBUG_PRINT_HEX_WORD(args->a4);
	DEBUG_PRINT_STR("\n A5:");
	DEBUG_PRINT_HEX_WORD(args->a5);
	DEBUG_PRINT_STR("\n A6:");
	DEBUG_PRINT_HEX_WORD(args->a6);
	DEBUG_PRINT_STR("\n A7:");
	DEBUG_PRINT_HEX_WORD(a7);
}


void mos_event_raise(uint8_t eventno, uint8_t X, uint8_t Y) {
	//TODO
}

void mos_default_IRQ1V() {


//			lda	SYS_VIA_IFR			; Read System VIA interrupt flag register
//			bpl	_POLL_USER_VIA_IRQ		; No System VIA interrupt, jump to check User VIA
	uint8_t sysviaifr = sheila_SYSVIA_ifr;
	if (sysviaifr & 0x80) {
//
//; System VIA interupt
//
//			and	OSB_SVIA_IRQ_M			; Mask with System VIA bit mask
//			and	SYS_VIA_IER			; and interrupt enable register
//_POLL_FRAME_SYNC_IRQ:	ror					; Rotate to check for CA1 interupt (frame sync)
//			ror					; 
//			bcc	_POLL_TIMER2_IRQ		; No CA1 (frame sync), jump to check speech

		sysviaifr = sysviaifr & sheila_SYSVIA_ier; //TODO: *OSB_VIA_IER mask from mos vars
		if (sysviaifr & VIA_IxR_CA1)
		{
		//TODO:CFS?
//
//; System VIA CA1 interupt (Frame Sync)
//
//			dec	OSB_CFS_TIMEOUT			; decrement vertical sync counter
//			lda	RS423_TIMEOUT			; A=RS423 Timeout counter
//			bpl	_BDD1E				; if +ve then DD1E
//			inc	RS423_TIMEOUT			; else increment it

			if (OSB_FLASH_TIME) {
				if (!(--OSB_FLASH_TIME)) {
					uint8_t v = OSB_VIDPROC_CTL;
					if (v & 1) 
						OSB_FLASH_TIME = OSB_FLASH_MARK;
					else
						OSB_FLASH_TIME = OSB_FLASH_SPC;
					vidula_set(v ^ 1);
				}
			}

			mos_event_raise(EVENT_04_VSYNC, 0, 0);

			sheila_SYSVIA_ifr = VIA_IxR_CA1;
			return;
		} else if (sysviaifr & VIA_IxR_T2) {

			/* TIMER 2 - TODO */

			sheila_SYSVIA_ifr = VIA_IxR_T2;
			return;
		} else if (sysviaifr & VIA_IxR_T1) {

			sheila_SYSVIA_ifr = VIA_IxR_T1;

			/* TIMER 1 - 100Hz tick */

			//TIME=
			uint8_t sw = OSB_TIME_SWITCH;
			uint8_t sw2 = sw ^ 0xF;
			uint8_t c = 1;
			OSB_TIME_SWITCH = sw2;
			volatile uint8_t *p = TIME_VAL1_MSB + sw - 1;
			volatile uint8_t *q = TIME_VAL1_MSB + sw2 - 1;
			uint8_t n = 5;
			while (n) {
				uint8_t v = *q + c;
				c = (c && v == 0)?1:0;
				*p = v;
				p--;
				q--;
				n--;
			}

			//TODO: INKEY TIMER
			//TODO: SOUND TIMER
			//TODO: BUFFER 8 SPEECH EMPTY
			//TODO: POLL ACIA

			if ((KEYNUM_FIRST | KEYNUM_LAST) & OSB_KEY_SEM) {
				call_KEYV_NVC();
			}

			//TODO: check ADC ready

		} else if (sysviaifr & VIA_IxR_CB1) {
			//ADC EOC
			sheila_SYSVIA_ifr = VIA_IxR_CB1;
			//TODO: ADC EOC
		} else if (sysviaifr & VIA_IxR_CA2) {
			//KEY PRESS
			sheila_SYSVIA_ifr = VIA_IxR_CA2;
			call_KEYV_NV();
		}
	}
}

//TODO: maybe move this out of main mos into buffers or somewhere?
void mos_default_WRCHV(uint8_t c) {
	//TODO: redirection and interception
	vdu_write(c);
}

void mos_reset(void) {
	DEBUG_PRINT_STR("INT INIT\n");
	interrupts_init();

	// disable SYSVIA interrupts
	sheila_SYSVIA_ier = 0x7F;
	sheila_SYSVIA_ifr = 0x7F;
	// disable USRVIA interrupts
	sheila_USRVIA_ier = 0x7F;
	sheila_USRVIA_ifr = 0x7F;


	// setup 100Hz tick
	sheila_SYSVIA_t1ll = T1PER & 0xFF;
	sheila_SYSVIA_t1lh = T1PER >> 8;
	sheila_SYSVIA_t1ch = T1PER >> 8;

	//TODO: this only resets a handful of things, should be table driven like on 6502
	OSB_TIME_SWITCH = 5;
	for (int i = 0 ; i < 10 ; i++) {
		TIME_VAL1_MSB[i] = i;
	}

	//resettable latch
	sheila_SYSVIA_ddrb = 0x0F;
	for (uint8_t i = 0xf; i>=9; i++)
		mos_latch_write(i);

	//init  variables TODO make this tabular
	KEYNUM_LAST = 0;
	KEYNUM_FIRST = 0;
	KEY_ROLLOVER_1 = 0;
	KEY_ROLLOVER_1A = 0;
	KEY_ROLLOVER_2 = 0;
	OSB_KEY_SEM = 0xFF;
	OSB_KEY_STATUS = 0x20;
	OSB_KEY_REPEAT = 2;
	OSB_KEY_DELAY = 30;

	OSB_ESC_BRK = 0;
	OSB_ESCAPE = 27;
	OSB_ESC_ACTION = 0;
	OSB_ESC_EFFECTS = 0;

	OSB_FLASH_MARK = 19;
	OSB_FLASH_SPC = 19;
	OSB_FLASH_TIME = OSB_FLASH_MARK;


	OSB_VDU_QSIZE = 0;

	ESCAPE_FLAG = 0;
	OSB_IN_STREAM = 0;

	VDU_ADJUST = 0;
	VDU_INTERLACE = 0;


	// renable T1, T2, EOC, VS
	//sheila_SYSVIA_ier = VIA_IxR_FLAG|VIA_IxR_T1|VIA_IxR_T2|VIA_IxR_CB1|VIA_IxR_CA1|VIA_IxR_CA2;
	sheila_SYSVIA_ier = VIA_IxR_FLAG|VIA_IxR_T1|VIA_IxR_T2|VIA_IxR_CA1|VIA_IxR_CA2; // not CB1
//	sheila_SYSVIA_pcr = VIA_PCR_CB2_CTL_IN_NEG|VIA_PCR_CB1_CTL_INT_NEG|VIA_PCR_CA2_CTL_IN_POS|VIA_PCR_CA1_CTL_INT_NEG;
//	sheila_SYSVIA_acr = VIA_ACR_T1_CTL_CONT_NOPB7|VIA_ACR_T2_CTL_PULSE_PB6|VIA_ACR_PB_LATCH_OFF|VIA_ACR_PA_LATCH_OFF;
//
//	sheila_USRVIA_pcr = VIA_PCR_CB2_CTL_IN_NEG|VIA_PCR_CB1_CTL_INT_NEG|VIA_PCR_CA2_CTL_OUT_HIGH|VIA_PCR_CA1_CTL_INT_NEG;

	DEBUG_PRINT_STR("BUF INIT\n");
	buffers_init();

	INSV = buffers_default_INSV;
	REMV = buffers_default_REMV;
	CNPV = buffers_default_CNPV;
	RDCHV = buffers_default_RDCHV;
	BYTEV = osbyte_default_BYTEV;
	WORDV = osword_default_WORDV;
	WRCHV = mos_default_WRCHV;
	VDUV = vdu_default_VDUV;

	//VDU font explode - TODO: make this like master
	VDU_FONT_FLAGS = 0x0F;
	memset(VDU_FONT_LOC, 8, 7);

	DEBUG_PRINT_STR("VDU INIT\n");
	vdu_init(4);

	sheila_SYSVIA_ifr = 0x7F; //clear all

	DEBUG_PRINT_STR("INT DIS 0\n");
	interrupts_disable(0);
	key_set_LEDs();

	DEBUG_PRINT_STR("GO\n");

}

