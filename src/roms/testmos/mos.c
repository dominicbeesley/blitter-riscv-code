#include "mos.h"
#include "hardware.h"
#include "interrupts.h"
#include "keyboard.h"
#include <stdlib.h>

//TODO:remove
extern void printstr(const char *s);
extern void printch(char c);
extern void hexword(unsigned int n);
extern void hexbyte(unsigned int n);



//Timer 1 100Hz period
#define T1PER (10000-2)

void mos_latch_write(uint8_t val) {
	//delay for at least 2us to ensure latch clocks
	*sheila_USRVIA_ddra;	// this should be enough
	*sheila_USRVIA_ddra;	// this should be enough

	*sheila_SYSVIA_orb = val;
}

void mos_enter_ecall(struct mos_args *args, uint32_t a7) {
	//asm("ebreak");
	//TODO: check for 0xAC0000?
	switch (a7 & 0xFF) {
		case OS_WRCH:
			printch(args->a0);
			return;
		case OS_NEWL:
			printstr("\n\r");
			return;

	}


	printstr(" A0:");
	hexword(args->a0);
	printstr(" A1:");
	hexword(args->a1);
	printstr(" A2:");
	hexword(args->a2);
	printstr(" A3:");
	hexword(args->a3);
	printstr(" A4:");
	hexword(args->a4);
	printstr(" A5:");
	hexword(args->a5);
	printstr(" A6:");
	hexword(args->a6);
	printstr(" A7:");
	hexword(a7);
}


void mos_event_raise(uint8_t eventno, uint8_t X, uint8_t Y) {
	//TODO
}

void mos_default_IRQ1V() {


//			lda	SYS_VIA_IFR			; Read System VIA interrupt flag register
//			bpl	_POLL_USER_VIA_IRQ		; No System VIA interrupt, jump to check User VIA
	uint8_t sysviaifr = *sheila_SYSVIA_ifr;
	if (sysviaifr & 0x80) {
//
//; System VIA interupt
//
//			and	OSB_SVIA_IRQ_M			; Mask with System VIA bit mask
//			and	SYS_VIA_IER			; and interrupt enable register
//_POLL_FRAME_SYNC_IRQ:	ror					; Rotate to check for CA1 interupt (frame sync)
//			ror					; 
//			bcc	_POLL_TIMER2_IRQ		; No CA1 (frame sync), jump to check speech

		sysviaifr = sysviaifr & *sheila_SYSVIA_ier; //TODO: *OSB_VIA_IER mask from mos vars
		if (sysviaifr & VIA_IxR_CA1)
		{
			//TODO:
//
//; System VIA CA1 interupt (Frame Sync)
//
//			dec	OSB_CFS_TIMEOUT			; decrement vertical sync counter
//			lda	RS423_TIMEOUT			; A=RS423 Timeout counter
//			bpl	_BDD1E				; if +ve then DD1E
//			inc	RS423_TIMEOUT			; else increment it
//_BDD1E:			lda	OSB_FLASH_TIME			; load flash counter
//			beq	_BDD3D				; if 0 then system is not in use, ignore it
//			dec	OSB_FLASH_TIME			; else decrement counter
//			bne	_BDD3D				; and if not 0 go on past reset routine
//
//			ldx	OSB_FLASH_SPC			; else get mark period count in X
//			lda	OSB_VIDPROC_CTL			; current VIDEO ULA control setting in A
//			lsr					; shift bit 0 into C to check if first colour
//			bcc	_BDD34				; is effective if so C=0 jump to DD34
//
//			ldx	OSB_FLASH_MARK			; else get space period count in X
//_BDD34:			rol					; restore bit
//			eor	#$01				; and invert it
//			jsr	VID_ULA_SET				; then change colour
//
//			stx	OSB_FLASH_TIME			; &0251=X resetting the counter
//
//_BDD3D:			ldy	#$04				; Y=4 and call E494 to check and implement vertical
//			jsr	_OSEVEN				; sync event (4) if necessary


//			lda	#$02				; A=2
//			jmp	_LDE6E				; clear interrupt 1 and exit
			*sheila_SYSVIA_ifr = VIA_IxR_CA1;
			return;
		} else if (sysviaifr & VIA_IxR_T2) {

			/* TIMER 2 - TODO */

			*sheila_SYSVIA_ifr = VIA_IxR_T2;
			return;
		} else if (sysviaifr & VIA_IxR_T1) {

			*sheila_SYSVIA_ifr = VIA_IxR_T1;

			/* TIMER 1 - 100Hz tick */

			//TIME=
			uint8_t sw = *OSB_TIME_SWITCH;
			uint8_t sw2 = sw ^ 0xF;
			uint8_t c = 1;
			*OSB_TIME_SWITCH = sw2;
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

			if ((*KEYNUM_FIRST | *KEYNUM_LAST) & *OSB_KEY_SEM) {
				call_KEYV_NVC();
			}

			//TODO: check ADC ready

		} else if (sysviaifr & VIA_IxR_CB1) {
			//ADC EOC
			*sheila_SYSVIA_ifr = VIA_IxR_CB1;
			//TODO: ADC EOC
		} else if (sysviaifr & VIA_IxR_CA2) {
			//KEY PRESS
			*sheila_SYSVIA_ifr = VIA_IxR_CA2;
			call_KEYV_NV();
		}
	}
}


void mos_reset(void) {
	// disable SYSVIA interrupts
	*sheila_SYSVIA_ier = 0x7F;
	*sheila_SYSVIA_ifr = 0x7F;


	// setup 100Hz tick
	*sheila_SYSVIA_t1ll = T1PER & 0xFF;
	*sheila_SYSVIA_t1lh = T1PER >> 8;
	*sheila_SYSVIA_t1ch = T1PER >> 8;

	//TODO: this only resets a handful of things, should be table driven like on 6502
	*OSB_TIME_SWITCH = 5;
	for (int i = 0 ; i < 10 ; i++) {
		TIME_VAL1_MSB[i] = i;
	}

	//resettable latch
	*sheila_SYSVIA_ddrb = 0x0F;
	for (uint8_t i = 0xf; i>=9; i++)
		mos_latch_write(i);

	//init keyboard
	*KEYNUM_LAST = 0;
	*KEYNUM_FIRST = 0;
	*KEY_ROLLOVER_1 = 0;
	*KEY_ROLLOVER_2 = 0;
	*OSB_KEY_SEM = 0xFF;
	*OSB_KEY_STATUS = 0x20;
	*OSB_KEY_REPEAT = 24;
	*OSB_KEY_DELAY = 40;


	// renable T1, T2, EOC, VS
	*sheila_SYSVIA_ier = VIA_IxR_FLAG|VIA_IxR_T1|VIA_IxR_T2|VIA_IxR_CB1|VIA_IxR_CA2;

	interrupts_disable(0);

}

