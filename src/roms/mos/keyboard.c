#include <stdint.h>
#include "mos.h"
#include "hardware.h"
#include <stddef.h>
#include "mos_shared.h"
#include "buffers.h"
#include "debug_print.h"

//TODO: check out CTRL-SHIFT-fX not working correctly?
//TODO: N-key rollover broken: autorepeat cancels, hammering keyboard cancels/sticks




// TODO: this is of a different form to the one in the original MOS where 
// rows are 16 bytes long - perhaps revert to that form if we reinstate
// the keyboard translation table pointer stuff

const uint8_t KEY_TRANS_TABLE[] = {
'q', '3', '4', '5', 0x84,'8', 0x87,'-', '^', 0x8c,
0x80,'w', 'e', 't', '7', 'i', '9', '0', '_', 0x8e,
'1', '2', 'd', 'r', '6', 'u', 'o', 'p', '[', 0x8f,
0x01,'a', 'x', 'f', 'y', 'j', 'k', '@', ':', 0x0d,
0x02,'s', 'c', 'g', 'h', 'n', 'l', ';', ']', 0x7f,
0x00,'z', ' ', 'v', 'b', 'm', ',', '.', '/', 0x8b,
0x1b,0x81,0x82,0x83,0x85,0x86,0x88,0x89,'\\',0x8d,
};



void key_scan_enable() {
	mos_latch_write(LATCH_8_ON|LATCH_3_KEYSCAN);
}

void key_set_LEDs(void) {

	mos_latch_write(((OSB_KEY_STATUS & 0x10) >> 1) | LATCH_6_LED_CAPS);	// CAPS LED from bit 4
	mos_latch_write(((OSB_KEY_STATUS & 0x20) >> 2) | LATCH_7_LED_SHIFT);	// SHIFT LED from bit 5

	key_scan_enable();
}


void key_EEDA_housekeep() {
	if (!KEYNUM_LAST && !KEYNUM_FIRST) {
		//reenable keyboard interrupts
		sheila_SYSVIA_ier = VIA_IxR_FLAG|VIA_IxR_CA2;
		OSB_KEY_SEM = 0;
	} else {
		OSB_KEY_SEM = 0xFF;
	}
	key_set_LEDs();
}


uint8_t key_keyboard_scanX(uint8_t X)
{
	mos_latch_write(LATCH_3_KEYSCAN);	// disable keyscan
	DELAY1
	sheila_SYSVIA_ddra = 0x7F;		// set keyboard as all out except bit 7
	DELAY1
	sheila_SYSVIA_ora_nh = X;			// key to scan
	DELAY1
	return sheila_SYSVIA_ora_nh;		// return with top bit set
}

//TODO: I suspect splitting this into two OSBYTE/non-OSBYTE routines will be simpler and possibly shorter!
uint8_t key_keyboard_scan_fromXon(uint8_t flags, uint8_t X, uint8_t Y) {
	uint8_t A = X;
	if (X & FLAG_N) {
		X = key_keyboard_scanX(X);
		if (flags & FLAG_C)
		{
			key_scan_enable();
			return X;
		}
	}
	if (flags & FLAG_C) {
		//first time entry from OSBYTE
		//TODO: figure out correct constants here
		Y = 0xEE;
	}

	*((uint8_t *)(0x1df + Y)) = A;
	X = 9;
	do {
		key_scan_enable();			// TODO: reenable interrupts for some reason?		
		key_scan_enable();			// TODO: check if this is too much / enough delay
		sheila_SYSVIA_ddra = 0x7F;		// ready to poke keyboard
		DELAY1
		mos_latch_write(LATCH_3_KEYSCAN); 	// disable keyscan		
		DELAY1
		sheila_SYSVIA_ora_nh = 0x0F; 		// select non-existent column
		DELAY1
		sheila_SYSVIA_ifr = VIA_IxR_CA2; 	// clear pending interrupt
		DELAY1
		sheila_SYSVIA_ora_nh = X;		// set to the column of interest
		DELAY1
		if (sheila_SYSVIA_ifr & VIA_IxR_CA2) {
			A = X;
			do {
				if (A >= *((uint8_t *)(0x1df + Y))) {
					sheila_SYSVIA_ora_nh = A;
					if (sheila_SYSVIA_ora_nh & 0x80) {
						if (flags & FLAG_C) {
							key_scan_enable();
							X = A;
							return A;
						} else {
							uint8_t c = ((*(uint8_t *)(uint32_t)Y) ^ A) << 1;
							if (c > 1) {
								key_scan_enable();
								X = A;
								return A;
							}
						}
					} 
				}
				A += 0x10;
			} while (A < 0x80);			
		}

		(X)--;
	} while (!(X & 0x80));
	A = X;
	key_scan_enable();
	return A;

}

void key_LF01F(void) {
	// set autorepeat timer to trigger soon
	AUTO_REPEAT_TIMER = 1;
	KEY_REPEAT_CNT = OSB_KEY_DELAY;
}


void _key_LEFE9(void) {
	if (KEYNUM_LAST) {
		KEYNUM_LAST = key_keyboard_scanX(KEYNUM_LAST);
		if (!(KEYNUM_LAST & FLAG_N))
			KEYNUM_LAST = 0;
	}
	uint8_t sr = key_keyboard_scan_fromXon(0, 0x10, 0xEC);// TODO: check number significance and use constant
	if (sr & FLAG_N) {
		key_EEDA_housekeep();
		return;
	}
	KEYNUM_LAST = KEYNUM_FIRST;
	KEYNUM_FIRST = sr;
	key_LF01F();
	key_EEDA_housekeep();
}

void key_irq_keypressed() {
	key_keyboard_scanX(0);		//don't think this matters?
	if (!KEYNUM_FIRST) 
	{
		uint8_t c = key_keyboard_scan_fromXon(0, 0x10, 0xED);
		if (!(c & 0x80)) {
			KEYNUM_FIRST = c;
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
		sheila_SYSVIA_ier = VIA_IxR_CA2; //disable keyboard interrupts
		if (!(flags & FLAG_C)) {
			// V=1, C=0
			key_irq_keypressed();
			return 0;
		} 
		(OSB_KEY_SEM) ++;
		//fall through for keyboard poll
	} else {
		// V = 0
		if (flags & FLAG_C)
			return key_keyboard_scan_fromXon(FLAG_C, *X, 0);
	}
	// EF16
	// V=0, V=0 or V=1, C=1
	uint8_t stat = OSB_KEY_STATUS & ~(KEYSTAT_CTRL|KEYSTAT_SHIFT);  // mask out CTRL/SHIFT
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
	OSB_KEY_STATUS = stat;
	if (!KEYNUM_FIRST)
	{
		// no key currently pressed
		_key_LEFE9();
		return 0;
	} else {
		uint8_t kf2 = key_keyboard_scanX(KEYNUM_FIRST);
		flags &= ~FLAG_Z;
		flags |= (kf2 == KEYNUM_FIRST)?FLAG_Z:0;
		if (
			((kf2 & FLAG_N) && !(flags & FLAG_Z))
		|	(!(kf2 & FLAG_N))
		) {
			KEYNUM_FIRST = kf2;
			if (flags & FLAG_Z)
			{
				KEYNUM_FIRST = 0;
				key_LF01F();
			}
			_key_LEFE9();
		} else {
			(AUTO_REPEAT_TIMER)--;
			if (AUTO_REPEAT_TIMER == 0) {
				//auto repeat!
				AUTO_REPEAT_TIMER = KEY_REPEAT_CNT;
				KEY_REPEAT_CNT = OSB_KEY_REPEAT;
				stat = OSB_KEY_STATUS;
				if (KEYNUM_FIRST == KEY_SHIFT_LOCK)
				{
					stat ^= KEYSTAT_SHIFT_EN|KEYSTAT_SHIFT_LK;
					OSB_KEY_STATUS = stat;
					AUTO_REPEAT_TIMER = 0;
					_key_LEFE9();
					return 0;
				} else if (KEYNUM_FIRST == KEY_CAPS_LOCK) {
					stat |= KEYSTAT_SHIFT_EN|KEYSTAT_SHIFT_LK;
					if (ws0 & FLAG_N) {
						//shift pressed above
						stat |= KEYSTAT_CAPS_LK;
						stat ^= KEYSTAT_SHIFT_EN;
					}
					stat ^= KEYSTAT_SHIFT_EN|KEYSTAT_CAPS_LK;
					OSB_KEY_STATUS = stat;
					AUTO_REPEAT_TIMER = 0;
					_key_LEFE9();
					return 0;
				} else {
					// get ascii code
					int ix = (KEYNUM_FIRST & 0x7F) - 0x10;
					ix = (((ix & 0x70) >> 4) * 10) + (ix & 0x0F);
					uint8_t a = KEY_TRANS_TABLE[ix];
					if (a == 0) 
						a = OSB_TAB;
					if (OSB_KEY_STATUS & KEYSTAT_CTRL) {
						if (KEYNUM_LAST) {
							// cancel repeat
							key_LF01F();
							_key_LEFE9();
							return 0;
						} else {
							a = mos_ascii_apply_ctrl(a);
						}
					}
					int ss = 0; // flag to tell whether already shifted due to caps lock
					if (!(OSB_KEY_STATUS & KEYSTAT_SHIFT_LK)) {
						//shift lock is active
						a = mos_ascii_apply_shift(a);
					} else if (!(OSB_KEY_STATUS & KEYSTAT_CAPS_LK)) {
						//caps lock is active
						if (isalpha(a))
							a = mos_ascii_apply_shift(a);
						else
							ss = 1;
					}

					// complicated due to shift+capslk vs normal capslk
					if (
						(OSB_KEY_STATUS & KEYSTAT_SHIFT) 
					&& (ss || (OSB_KEY_STATUS & KEYSTAT_SHIFT_EN))
					) {
						if (KEYNUM_LAST) {
							// cancel repeat
							key_LF01F();
							_key_LEFE9();
							return 0;
						} else {
							a = mos_ascii_apply_shift(a);
						}						
					}

					// Test for ESCape

					if (a == OSB_ESCAPE && OSB_ESC_ACTION == 0)
						AUTO_REPEAT_TIMER = 0;

					//TODO: econet keyboard disable

					buffers_insert_check_ESC(BUFFER_KEYBOARD_IN, a);

					if (KEYNUM_LAST) {
						uint8_t knl = key_keyboard_scanX(KEYNUM_LAST);
						if (!(knl & 0x80))
							KEYNUM_LAST = 0x00;
					}

					if (KEYNUM_LAST) {
						if (!(key_keyboard_scan_fromXon(FLAG_N, KEYNUM_LAST, 0xEC) & 0x80)) {
							uint8_t t = KEYNUM_FIRST;
							KEYNUM_FIRST = KEYNUM_LAST;
							KEYNUM_LAST = t;
							key_LF01F();
						}
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

