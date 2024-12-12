#include "mos.h"
#include "hardware.h"
#include "interrupts.h"
#include <stdlib.h>

//TODO:remove
extern void printstr(const char *s);
extern void printch(char c);
extern void hexword(unsigned int n);
extern void hexbyte(unsigned int n);

const uint8_t const KEY_TRANS_TABLE[] = {
'q', '3', '4', '5', 0x84,'8', 0x87,'-', '^', 0x8c,
0x80,'w', 'e', 't', '7', 'i', '9', '0', '_', 0x8e,
'1', '2', 'd', 'r', '6', 'u', 'o', 'p', '[', 0x8f,
0x01,'a', 'x', 'f', 'y', 'j', 'k', '@', ':', 0x0d,
0x02,'s', 'c', 'g', 'h', 'n', 'l', ';', ']', 0x7f,
0x00,'z', ' ', 'v', 'b', 'm', ',', '.', '/', 0x8b,
0x1b,0x81,0x82,0x83,0x85,0x86,0x88,0x89,'\\',0x8d,
};


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

void key_scan_enable() {
	mos_latch_write(LATCH_8_ON|LATCH_3_KEYSCAN);
}

void key_set_LEDs(void) {

	mos_latch_write(((*OSB_KEY_STATUS & 0x10) >> 1) | LATCH_6_LED_CAPS);	// CAPS LED from bit 4
	mos_latch_write(((*OSB_KEY_STATUS & 0x20) >> 2) | LATCH_7_LED_SHIFT);	// SHIFT LED from bit 5

	key_scan_enable();
}


void key_EEDA_housekeep() {
	if (!*KEYNUM_LAST && !*KEYNUM_FIRST) {
		//reenable keyboard interrupts
		*sheila_SYSVIA_ier = VIA_IxR_FLAG|VIA_IxR_CA2;
		*OSB_KEY_SEM = 0;
	} else {
		*OSB_KEY_SEM = 0xFF;
	}
	key_set_LEDs();
}


uint8_t key_keyboard_scanX(uint8_t X)
{
	mos_latch_write(LATCH_3_KEYSCAN);	// disable keyscan
	*sheila_SYSVIA_ddra = 0x7F;		// set keyboard as all out except bit 7
	*sheila_SYSVIA_ora = X;			// key to scan
	return *sheila_SYSVIA_ora;		// return with top bit set
}

//TODO: I suspect splitting this into two OSBYTE/non-OSBYTE routines will be simpler and possibly shorter!
uint8_t key_keyboard_scan_fromXon(uint8_t flags, uint8_t *X, uint8_t Y) {
	uint8_t A = *X;
	if (*X & FLAG_N) {
		*X = key_keyboard_scanX(*X);
		if (flags & FLAG_C)
		{
			key_scan_enable();
			return *X;
		}
	}
	if (flags & FLAG_C) {
		//first time entry from OSBYTE
		//TODO: figure out correct constants here
		Y = 0xEE;
	}
	*((uint8_t *)(0x1df + Y)) = A;
	*X = 9;
	do {
		key_scan_enable();			// TODO: reenable interrupts for some reason?		
		key_scan_enable();			// TODO: check if this is too much / enough delay
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ddra = 0x7F;		// ready to poke keyboard
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		mos_latch_write(LATCH_3_KEYSCAN); 	// disable keyscan		
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh = 0x0F; 		// select non-existent column
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ifr = VIA_IxR_CA2; 	// clear pending interrupt
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh = *X;		// set to the column of interest
		//DELAY?
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		*sheila_SYSVIA_ora_nh;
		if (*sheila_SYSVIA_ifr & VIA_IxR_CA2) {
			A = *X;
			do {
				if (A >= *((uint8_t *)(0x1df + Y))) {
					*sheila_SYSVIA_ora_nh = A;
					if (*sheila_SYSVIA_ora_nh & 0x80) {
						if (flags & FLAG_C) {
							key_scan_enable();
							*X = A;
							return A;
						} else {
							uint8_t c = ((*(uint8_t *)(uint32_t)Y) ^ A) << 1;
							if (c > 1) {
								key_scan_enable();
								*X = A;
								return A;
							}
						}
					} 
				}
				A += 0x10;
			} while (A < 0x80);			
		}

		(*X)--;
	} while (!((*X) & 0x80));
	A = *X;
	key_scan_enable();
	return A;

}

void key_LF01F(void) {
	// set autorepeat timer to trigger soon
	*AUTO_REPEAT_TIMER = 1;
	*KEY_REPEAT_CNT = *OSB_KEY_DELAY;
}


void _key_LEFE9(void) {
	if (*KEYNUM_LAST) {
		*KEYNUM_LAST = key_keyboard_scanX(*KEYNUM_LAST);
		if (!(*KEYNUM_LAST & FLAG_N))
			*KEYNUM_LAST = 0;
	}
	uint8_t X = 0x10;
	uint8_t sr = key_keyboard_scan_fromXon(0, &X, 0xEC);// TODO: check number significance and use constant
	if (sr & FLAG_N) {
		key_EEDA_housekeep();
		return;
	}
	*KEYNUM_LAST = *KEYNUM_FIRST;
	*KEYNUM_FIRST = X;
	key_LF01F();
	key_EEDA_housekeep();
}

void key_irq_keypressed() {
	key_keyboard_scanX(0);		//don't think this matters?
	if (!*KEYNUM_FIRST) 
	{
		uint8_t X = 0x10;
		uint8_t c = key_keyboard_scan_fromXon(0, &X, 0xED);
		if (!(c & 0x80)) {
			*KEYNUM_FIRST = c;
			key_LF01F();			
		}		
	}
	key_EEDA_housekeep();
}

uint8_t mos_ascii_apply_BEAAC(uint8_t c) {
	c ^= 0x30;
	if (c == 0x6F || c == 0x50)
		c ^= 0x1F;
	if (c >= 0x21)
		c ^= 0x10;
	return c;
}

uint8_t mos_ascii_apply_shift(uint8_t c) {
	if (c != ' ' && c != '@' && c != 0x7F) {
		if (c < '@') {
			if (c >= 0x21)
				c ^= 0x10;
			return c;
		} else if (c > 0x7F) {
			c ^= 0x10;
			return c;
		}
		return mos_ascii_apply_BEAAC(c);
	}
	else
		return c;
}

uint8_t mos_ascii_apply_ctrl(uint8_t c) {
	if (c != 0x7F) {
		c = mos_ascii_apply_BEAAC(c);
		if (c == 0x60) c = 0x5F;
		else if (c >= 0x40) c &= 0x1F;
	}

	return c;
}

int isalpha(uint8_t c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <='Z');
}

uint8_t call_KEYV(uint8_t flags, uint8_t *X) {
	if (flags & FLAG_V) {
		*sheila_SYSVIA_ier = VIA_IxR_CA2; //disable keyboard interrupts
		if (!(flags & FLAG_C)) {
			// V=1, C=0
			key_irq_keypressed();
			return 0;
		} 
		(*OSB_KEY_SEM) ++;
		//fall through for keyboard poll
	} else {
		// V = 0
		if (flags & FLAG_C)
			key_keyboard_scan_fromXon(flags, X, 0);
	}
	// EF16
	// V=0, V=0 or V=1, C=1
	uint8_t stat = *OSB_KEY_STATUS & ~(KEYSTAT_CTRL|KEYSTAT_SHIFT);  // mask out CTRL/SHIFT
	uint8_t ws0 = key_keyboard_scanX(KEY_SHIFT);		
	flags &= ~FLAG_V;
	if (ws0 & FLAG_N)
	{
		flags |= FLAG_N|FLAG_V;
		stat |= KEYSTAT_SHIFT;
	}
	if (!(flags & FLAG_C)) {
		return (flags & ~FLAG_N) | (key_keyboard_scanX(KEY_CTRL) & FLAG_N);
	}
	if (key_keyboard_scanX(KEY_CTRL) & FLAG_N) {
		stat |= KEYSTAT_CTRL;
	}
	*OSB_KEY_STATUS = stat;
	if (!*KEYNUM_FIRST)
	{
		// no key currently pressed
		_key_LEFE9();
		return 0;
	} else {
		uint8_t kf2 = key_keyboard_scanX(*KEYNUM_FIRST);
		flags &= ~FLAG_Z;
		flags |= (kf2 == *KEYNUM_FIRST)?FLAG_Z:0;
		if (
			((kf2 & FLAG_N) && !(flags & FLAG_Z))
		|	(!(kf2 & FLAG_N))
		) {
			*KEYNUM_FIRST = kf2;
			if (flags & FLAG_Z)
			{
				*KEYNUM_FIRST = 0;
				key_LF01F();
			}
			_key_LEFE9();
		} else {
			(*AUTO_REPEAT_TIMER)--;
			if (*AUTO_REPEAT_TIMER == 0) {
				//auto repeat!
				*AUTO_REPEAT_TIMER = *KEY_REPEAT_CNT;
				*KEY_REPEAT_CNT = *OSB_KEY_REPEAT;
				stat = *OSB_KEY_STATUS;
				if (*KEYNUM_FIRST == KEY_SHIFT_LOCK)
				{
					stat ^= KEYSTAT_SHIFT_EN|KEYSTAT_SHIFT_LK;
					*OSB_KEY_STATUS = stat;
					*AUTO_REPEAT_TIMER = 0;
					_key_LEFE9();
					return 0;
				} else if (*KEYNUM_FIRST == KEY_CAPS_LOCK) {
					stat |= KEYSTAT_SHIFT_EN|KEYSTAT_SHIFT_LK;
					if (ws0 & FLAG_N) {
						//shift pressed above
						stat |= KEYSTAT_CAPS_LK;
						stat ^= KEYSTAT_SHIFT_EN;
					}
					stat ^= KEYSTAT_SHIFT_EN|KEYSTAT_CAPS_LK;
					*OSB_KEY_STATUS = stat;
					*AUTO_REPEAT_TIMER = 0;
					_key_LEFE9();
					return 0;
				} else {
					// get ascii code
					int ix = ((*KEYNUM_FIRST) & 0x7F) - 0x10;
					ix = (((ix & 0x70) >> 4) * 10) + (ix & 0x0F);
					uint8_t a = KEY_TRANS_TABLE[ix];
					if (a == 0) 
						a = *OSB_TAB;
					if (*OSB_KEY_STATUS & KEYSTAT_CTRL) {
						if (*KEYNUM_LAST) {
							// cancel repeat
							key_LF01F();
							_key_LEFE9();
							return 0;
						} else {
							a = mos_ascii_apply_ctrl(a);
						}
					}
					int ss = 0; // flag to tell whether already shifted due to caps lock
					if (!(*OSB_KEY_STATUS & KEYSTAT_SHIFT_LK)) {
						//shift lock is active
						a = mos_ascii_apply_shift(a);
					} else if (!(*OSB_KEY_STATUS & KEYSTAT_CAPS_LK)) {
						//caps lock is active
						if (isalpha(a))
							a = mos_ascii_apply_shift(a);
						else
							ss = 1;
					}

					// complicated due to shift+capslk vs normal capslk
					if (
						(*OSB_KEY_STATUS & KEYSTAT_SHIFT) 
					&& (ss || (*OSB_KEY_STATUS & KEYSTAT_SHIFT_EN))
					) {
						if (*KEYNUM_LAST) {
							// cancel repeat
							key_LF01F();
							_key_LEFE9();
							return 0;
						} else {
							a = mos_ascii_apply_shift(a);
						}						
					}

					//TODO:ESC ACTION

					if (a > 0x20 && a < 0x80) {
						printch(a);
					} else {
						hexbyte(a);
					}
					_key_LEFE9();
					return 0;
				}
			} else {
				_key_LEFE9();
			}
		}
	}
	return 0;

}












void call_KEYV_NVC() {
	//keyboard poll 100 times a second when key held
	call_KEYV(FLAG_N|FLAG_V|FLAG_C, NULL);
}

void call_KEYV_NV() {
	//key pressed interrupt
	call_KEYV(FLAG_N|FLAG_V, NULL);
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
	*OSB_KEY_REPEAT = 12;
	*OSB_KEY_DELAY = 20;


	key_set_LEDs();
	key_scan_enable();

	// renable T1, T2, EOC, VS
	*sheila_SYSVIA_ier = VIA_IxR_FLAG|VIA_IxR_T1|VIA_IxR_T2|VIA_IxR_CB1|VIA_IxR_CA2;

	interrupts_disable(0);

}

