#include "vdu.h"
#include "mos.h"
#include "mos_shared.h"
#include "interrupts.h"
#include "debug_print.h"

#include <string.h>

//this must be X^2-1
#define MAXMODES 7

const uint8_t _TBL_MODE_COLOURS[] = {
			// MODE 0 - 2 COLOURS
			0x01, 
			// MODE 1 - 4 COLOURS
			0x03, 
			// MODE 2 - 16 COLOURS
			0x0f, 
			// MODE 3 - 2 COLOURS
			0x01, 
			// MODE 4 - 2 COLOURS
			0x01, 
			// MODE 5 - 4 COLOURS
			0x03, 
			// MODE 6 - 2 COLOURS
			0x01, 
			// MODE 7 - 1 COLOUR
			0x00
	};

const uint8_t _TXT_BPC_TABLE[] = {
		0x08,
		0x10,
		0x20,
		0x08,
		0x08,
		0x10,
		0x08,
  		0x01
  	};

 //TODO: there's some savings here in mos rom by overlapping - bother?


const uint8_t _TBL_VDU_PIXPB[] = {
 	// MODE 0 - 8 PIXELS/BYTE
 	0x07,
	// MODE 1 - 4 PIXELS/BYTE
	0x03,
	// MODE 2 - 2 PIXELS/BYTE
	0x01,
	// MODE 3 - 1 PIXEL/BYTE (NON-GRAPHICS)
	0x00,
	// MODE 4 - 8 PIXELS/BYTE
	0x07,
	// MODE 5 - 4 PIXELS/BYTE
	0x03,
	// MODE 6 - 0 PIXELS/BYTE
	0x00,
	// MODE 5 - 0 PIXELS/BYTE
	0x00
};


//TODO: _TBL_VDU_PIXPB overlaps with _LC43D for saving - bother?

const uint8_t _TAB_MAP_TYPE[] = {
	0x00,
	0x00,
	0x00,
	0x01,
	0x02,
	0x02,
	0x03,
	0x04
};

const uint8_t _TBL_MASK_R[] = {
		// not really needed
		0x01,
		//2 PX PER BYTE
		0xAA,
		0x55,
		//4 PX PER BYTE
		0x88,
		0x44,
		0x22,
		0x11,
		//8 PX PER BYTE
		0x80,
		0x40,
		0x20,
		0x10,
		0x08,
		0x04,
		0x02,
		0x01
};

// value to write to 8 bit latch bit 4 indexed by mode size type (see _TAB_MAP_TYPE)
const uint8_t _TAB_LAT4_MOSZ[] = {
		0x0d,
		0x05,
		0x0d,
		0x05
	};

const uint8_t _TAB_LAT5_MOSZ[] = {
		0x04,
		0x04,
		0x0c,
		0x0c,
		0x04
	};


//************** MSB OF MEMORY OCCUPIED BY SCREEN BUFFER	 *****************

const uint8_t _VDU_MEMSZ_TAB[] = {
	// Type 0: &5000 - 20K
	0x50,
	// Type 1: &4000 - 16K
	0x40,
	// Type 2: &2800 - 10K
	0x28,
	// Type 3: &2000 - 8K
	0x20,
	// Type 4: &0400 - 1K
	0x04
};


//************ MSB OF FIRST LOCATION OCCUPIED BY SCREEN BUFFER ************

const uint8_t _VDU_MEMLOC_TAB[] = {
	// Type 0: &3000
	0x30,
	// Type 1: &4000
	0x40,
	// Type 2: &5800
	0x58,
	// Type 3: &6000
	0x60,
	// Type 4: &7C00
	0x7c
};

// NOTE: layout is not he same as MOS which does some sort of shiftiness
const uint16_t _TAB_BPR[] = {
	640,
	640,
	320,
	320,
	40
};


//************** VIDEO ULA CONTROL REGISTER SETTINGS ***********************

const uint8_t _ULA_SETTINGS[] = {
	0x9c,
	0xd8,
	0xf4,
	0x9c,
	0x88,
	0xc4,
	0x88,
	0x4b
};

const uint8_t _TAB_CRTCBYSZ[5][12] = {
	{
		// 6845 REGISTERS 0-11 FOR SCREEN TYPE 0 - MODES 0-2
		0x7f,	// 0 Horizontal Total	 =128
		0x50,	// 1 Horizontal Displayed =80
		0x62,	// 2 Horizontal Sync	 =&62
		0x28,	// 3 HSync Width+VSync	 =&28  VSync=2, HSync Width=8
		0x26,	// 4 Vertical Total	 =38
		0x00,	// 5 Vertial Adjust	 =0
		0x20,	// 6 Vertical Displayed	 =32
		0x22,	// 7 VSync Position	 =&22
		0x01,	// 8 Interlace+Cursor	 =&01  Cursor=0, Display=0, Interlace=Sync
		0x07,	// 9 Scan Lines/Character =8
		0x67,	// 10 Cursor Start Line	  =&67	Blink=On, Speed=1/32, Line=7
		0x08	// 11 Cursor End Line	  =8
	},
	{
		// 6845 REGISTERS 0-11 FOR SCREEN TYPE 1 - MODE 3
		0x7f,	// 0 Horizontal Total	 =128
		0x50,	// 1 Horizontal Displayed =80
		0x62,	// 2 Horizontal Sync	 =&62
		0x28,	// 3 HSync Width+VSync	 =&28  VSync=2, HSync=8
		0x1e,	// 4 Vertical Total	 =30
		0x02,	// 5 Vertical Adjust	 =2
		0x19,	// 6 Vertical Displayed	 =25
		0x1b,	// 7 VSync Position	 =&1B
		0x01,	// 8 Interlace+Cursor	 =&01  Cursor=0, Display=0, Interlace=Sync
		0x09,	// 9 Scan Lines/Character =10
		0x67,	// 10 Cursor Start Line	  =&67	Blink=On, Speed=1/32, Line=7
		0x09	// 11 Cursor End Line	  =9
	}, {
		// 6845 REGISTERS 0-11 FOR SCREEN TYPE 2 - MODES 4-5

		0x3f,	// 0 Horizontal Total	 =64
		0x28,	// 1 Horizontal Displayed =40
		0x31,	// 2 Horizontal Sync	 =&31
		0x24,	// 3 HSync Width+VSync	 =&24  VSync=2, HSync=4
		0x26,	// 4 Vertical Total	 =38
		0x00,	// 5 Vertical Adjust	 =0
		0x20,	// 6 Vertical Displayed	 =32
		0x22,	// 7 VSync Position	 =&22
		0x01,	// 8 Interlace+Cursor	 =&01  Cursor=0, Display=0, Interlace=Sync
		0x07,	// 9 Scan Lines/Character =8
		0x67,	// 10 Cursor Start Line	  =&67	Blink=On, Speed=1/32, Line=7
		0x08	// 11 Cursor End Line	  =8
	}, {
		// 6845 REGISTERS 0-11 FOR SCREEN TYPE 3 - MODE 6
		0x3f,	// 0 Horizontal Total	 =64
		0x28,	// 1 Horizontal Displayed =40
		0x31,	// 2 Horizontal Sync	 =&31
		0x24,	// 3 HSync Width+VSync	 =&24  VSync=2, HSync=4
		0x1e,	// 4 Vertical Total	 =30
		0x02,	// 5 Vertical Adjust	 =0
		0x19,	// 6 Vertical Displayed	 =25
		0x1b,	// 7 VSync Position	 =&1B
		0x01,	// 8 Interlace+Cursor	 =&01  Cursor=0, Display=0, Interlace=Sync
		0x09,	// 9 Scan Lines/Character =10
		0x67,	// 10 Cursor Start Line	  =&67	Blink=On, Speed=1/32, Line=7
		0x09	// 11 Cursor End Line	  =9
	}, {

		// 6845 REGISTERS 0-11 FOR SCREEN TYPE 4 - MODE 7 
		0x3f,	// 0 Horizontal Total	 =64
		0x28,	// 1 Horizontal Displayed =40
		0x33,	// 2 Horizontal Sync	 =&33  Note: &31 is a better value
		0x24,	// 3 HSync Width+VSync	 =&24  VSync=2, HSync=4
		0x1e,	// 4 Vertical Total	 =30
		0x02,	// 5 Vertical Adjust	 =2
		0x19,	// 6 Vertical Displayed	 =25
		0x1b,	// 7 VSync Position	 =&1B
		0x93,	// 8 Interlace+Cursor	 =&93  Cursor=2, Display=1, Interlace=Sync+Video
		0x12,	// 9 Scan Lines/Character =19
		0x72,	// 10 Cursor Start Line	  =&72	Blink=On, Speed=1/32, Line=18
		0x13	// 11 Cursor End Line	  =19
	}
};


// ****** TEXT WINDOW -BOTTOM ROW LOOK UP TABLE ******

const uint8_t _TEXT_ROW_TABLE[] = {
	//MODE 0 - 32 ROWS
	0x1f,
	//MODE 1 - 32 ROWS
	0x1f,
	//MODE 2 - 32 ROWS
	0x1f,
	//MODE 3 - 25 ROWS
	0x18,
	//MODE 4 - 32 ROWS
	0x1f,
	//MODE 5 - 32 ROWS
	0x1f,
	//MODE 6 - 25 ROWS
	0x18,
	//MODE 7 - 25 ROWS
	0x18
};


// ****** TEXT WINDOW -RIGHT HAND COLUMN LOOK UP TABLE ******

const uint8_t _TEXT_COL_TABLE[] = {
	// MODE 0 - 80 COLUMNS
	0x4f,
	// MODE 1 - 40 COLUMNS
	0x27,
	// MODE 2 - 20 COLUMNS
	0x13,
	// MODE 3 - 80 COLUMNS
	0x4f,
	// MODE 4 - 40 COLUMNS
	0x27,
	// MODE 5 - 20 COLUMNS
	0x13,
	// MODE 6 - 40 COLUMNS
	0x27,
	// MODE 7 - 40 COLUMNS
	0x27
};





void vidula_set(uint8_t n) {
	uint32_t irq = interrupts_disable(INT_IRQ);

	OSB_VIDPROC_CTL = n;
	sheila_VIDULA_CTL = n;
	OSB_FLASH_TIME = OSB_FLASH_MARK;	

	interrupts_disable(irq);
}

void crtc_poke(uint8_t index, uint8_t val) {
	if (index == 7)
		val += VDU_ADJUST;
	else if (index == 8 && !(val & 0x80))
		val ^= VDU_INTERLACE;
	else if (index == 10)
	{
		VDU_CURS_PREV = val;
		if (VDU_STATUS & VDUSTATUS_GFX)
			return;
	}

	sheila_CRTC_ix = index;
	sheila_CRTC_dat = val;
}

void vdu_osbyte_154_SET_VID_ULA(uint8_t X) {
	vidula_set(X);
}


void vdu_osbyte_20_EXPLODE(void) {
	//TODO: explosions...ignore?
}

void _LEA11_write_pal(uint8_t n) {
	uint32_t irq = interrupts_disable(INT_IRQ);

	n ^= 7;		//convert to ULA colour numbers
	OSB_VIDPROC_PAL = n;		// pointless?
	sheila_VIDULA_PAL = n;

	interrupts_disable(irq);
}

void vdu_osbyte_155_SET_PALETTE(uint8_t X) {
	_LEA11_write_pal(X);
}

void pal_write(uint8_t ix, uint8_t val) {
	ix = ix & VDU_COL_MASK;
	val = val & 0xF;
	
	VDU_PALETTE[ix] = val;
	uint8_t ws0 = VDU_COL_MASK;

	int bpp = 0;
	while (ws0 & 1) {
		ws0 = (ws0 >> 1);
		bpp++;
	}

	ws0 = ix << (8 - bpp);
	ws0 = ws0 | val;

	uint8_t y = 0;
	do {
		uint8_t A = ws0;
		if (VDU_COL_MASK == 3)
		{
			if ((ws0 & 0x60) && ((ws0 &0x60) != 0x60)) 
				A ^= 0x60;
		}
		_LEA11_write_pal(A);
		y += VDU_COL_MASK + 1;
		ws0 += 0x10;
	} while (y < 16);
}

/*************************************************************************
 *									 *
 *	 VDU 19 - DEFINE COLOURS					 *
 *	[COLOUR l,p]							 *
 *	[COLOUR l,r,g,b]						 *
 *									 *
 *	 5 parameters							 *
 *									 *
 *************************************************************************
 Q[4]=first parameter logical colour
 Q[5]=second physical colour
*/


void VDU_19(void) {
	//TODO: why does VDU_19 disable interrupts and not OSWORD 12?
	uint32_t irq = interrupts_disable(INT_IRQ);
	
	pal_write(VDU_QUEUE[4], VDU_QUEUE[5]);

	interrupts_disable(irq);
}

/*************************************************************************
 *									 *
 *	 VDU 20 - RESTORE DEFAULT COLOURS				 *
 *									 *
 *************************************************************************/

void VDU_20(void) {
	//TODO: look at reduce size?
	VDU_T_FG = 0;
	VDU_T_BG = 0xA5;
	VDU_G_FG = 0;
	VDU_G_BG = 0;
	VDU_P_FG = 0;
	VDU_P_BG = 0;

	if (VDU_COL_MASK == 0)
	{
		//mode 7
		VDU_T_BG = ' ';
		return;
	} 
	uint8_t m = (VDU_COL_MASK == 0xF)?0x3F:0xFF; // white (no flash in mode 2)
	VDU_T_FG = m;
	VDU_G_FG = m;
	m = m ^ 0xFF;
	VDU_T_OR_MASK = m;
	VDU_T_EOR_MASK = m;

	int8_t c = VDU_COL_MASK;
	if (c == 3) {
		VDU_QUEUE[4] = 3;
		VDU_QUEUE[5] = 7;
		do {
			VDU_19();
			VDU_QUEUE[5] >>= 1;
			VDU_QUEUE[4] --;
		} while (!(VDU_QUEUE[4] & 0x80));
	} else if (c < 3) {
		VDU_QUEUE[4] = 1;
		VDU_QUEUE[5] = 7;
		VDU_19();
		VDU_QUEUE[4] = 0;
		VDU_QUEUE[5] = 0;
		VDU_19();
	} else {
		// 16 colour mode
		for (c = 0; c < 16; c++) {
			VDU_QUEUE[4] = c;
			VDU_QUEUE[5] = c;
			VDU_19();
		}					
	}


}

void _LCA88_CALC_BYTES_PER_LINE(uint8_t widthm1) {
	int width = widthm1 + 1;
	VDU_T_WIN_SZ = width * VDU_BPC;
}

uint16_t SET_CRTC_16(uint8_t ix, uint16_t addr) {
	crtc_poke(ix, addr >> 8);
	crtc_poke(ix+1, addr);
	return addr;
}

uint16_t SET_CRTC_16DIV8(uint8_t ix, uint16_t addr) {
	SET_CRTC_16(ix, addr >> 3);
	return addr >> 3;
}

uint16_t SET_CUR_CHARSCANX(uint16_t addr) {
	VDU_TOP_SCAN = addr;	
	return SET_CRTC_16DIV8(0xE, VDU_CRTC_CUR);
}


uint16_t SET_CRTC_CURS16_adj(uint16_t addr) {
	VDU_CRTC_CUR = addr;
	if (addr & 0x8000)
		addr -= VDU_MEM_PAGES << 8;
	return SET_CUR_CHARSCANX(addr);
}

/*************************************************************************
 *									 *
 *	 VDU 24 - DEFINE GRAPHICS WINDOW				 *
 *									 *
 *	 8 parameters							 *
 *									 *
 *************************************************************************
 &31C/D Left margin
 &31E/F Bottom margin
 &320/1 Right margin
 &322/3 Top margin
*/
void VDU_24(void) {
	//TODO: GRAPHICS
}


/*************************************************************************
 *									 *
 *	 VDU 26 - SET DEFAULT WINDOWS					 *
 *									 *
 *************************************************************************/

void VDU_26(void) {
	//this looks to clear way more than it should but the code below relies on it - TODO: make more sane?
	memset(&VDU_G_WIN_L, 0, 0x2C);	

	VDU_T_WIN_R = _TEXT_COL_TABLE[VDU_MODE];
	_LCA88_CALC_BYTES_PER_LINE(VDU_T_WIN_R);
	VDU_T_WIN_B = _TEXT_ROW_TABLE[VDU_MODE];
	VDU_QUEUE[8] = (1023 >> 8);
	VDU_QUEUE[7] = (1023 & 0xFF);
	VDU_QUEUE[6] = (1279 >> 8);
	VDU_QUEUE[5] = (1279 & 0xFF);
	VDU_24();
	VDU_STATUS &= ~VDUSTATUS_SOFT_SCROLL; // hard scroll
	SET_CRTC_CURS16_adj(VDU_MEM);
}

void _LCBC1_DOCLS() {
	VDU_MEM = (uint16_t)VDU_PAGE << 8;
	uint16_t crtcdiv8 = SET_CRTC_CURS16_adj(VDU_MEM);
	SET_CRTC_16(0xC, crtcdiv8);
	OSB_HALT_LINES = 0;
	VDU_T_CURS_X = 0;
	VDU_T_CURS_Y = 0;

	DEBUG_PRINT_STR("CLS");
	DEBUG_PRINT_HEX_WORD((0xFFFF0000 | (uint32_t)VDU_MEM));
	DEBUG_PRINT_HEX_WORD(((uint16_t)VDU_MEM_PAGES << 8));

	memset((void *)(0xFFFF0000 | (uint32_t)VDU_MEM), VDU_T_BG, (uint16_t)VDU_MEM_PAGES << 8);

}

void vdu_mode(uint8_t mode) {
	mode = mode & MAXMODES;
	VDU_MODE = mode;
	VDU_COL_MASK = _TBL_MODE_COLOURS[mode];
	VDU_BPC = _TXT_BPC_TABLE[mode];
	VDU_PIX_BYTE = _TBL_VDU_PIXPB[mode];
	uint8_t ll = VDU_MASK_RIGHT = _TBL_MASK_R[VDU_PIX_BYTE?(VDU_PIX_BYTE<<1):14];
	while (!(ll & 0x80)) ll = ll << 1;
	VDU_MASK_LEFT = ll;

	uint8_t t = VDU_MAP_TYPE = _TAB_MAP_TYPE[mode];
	mos_latch_write(_TAB_LAT5_MOSZ[t]);	
	mos_latch_write(_TAB_LAT4_MOSZ[t]);	
	VDU_MEM_PAGES = _VDU_MEMSZ_TAB[t];
	VDU_PAGE = _VDU_MEMLOC_TAB[t];
	VDU_BPR = _TAB_BPR[t];
	vidula_set(_ULA_SETTINGS[mode]);
	for (int i = 0; i < 12; i++) {
		crtc_poke(i, _TAB_CRTCBYSZ[t][i]);
	}
	VDU_20();
	VDU_26();
	_LCBC1_DOCLS();
}

void vdu_init(uint8_t mode) {
	VDU_STATUS = 0;
	memset(&VDU_VARS_BASE, 0, VDU_VARS_SIZE);
	vdu_osbyte_20_EXPLODE();
	VDU_TTX_CURSOR = 0x7F;
	vdu_mode(mode);
}

void vdu_write(uint8_t c) {

}

