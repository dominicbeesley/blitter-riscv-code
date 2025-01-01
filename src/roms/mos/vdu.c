#include "vdu.h"
#include "mos.h"
#include "mos_shared.h"
#include "interrupts.h"
#include "debug_print.h"
#include "font.h"
#include <assert.h>

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

VDUV_FN VDUV;


//this must be X^2-1
#define MAXMODES 7

#define SCREENADDR(A) (uint8_t *)(0xFFFF0000 | ((uint32_t)A))

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

const uint8_t _LC432_COLOURS_TAB[] = {
//********** 2 COLOUR MODES PARAMETER LOOK UP TABLE WITHIN TABLE **********

    0x00,
    0xff,


//*************** 4 COLOUR MODES PARAMETER LOOK UP TABLE ******************

    0x00,
    0x0f,
    0xf0,
    0xff,


//***************16 COLOUR MODES PARAMETER LOOK UP TABLE ******************

    0x00,
    0x03,
    0x0c,
    0x0f,
    0x30,
    0x33,
    0x3c,
    0x3f,
    0xc0,
    0xc3,
    0xcc,
    0xcf,
    0xf0,
    0xf3,
    0xfc,
    0xff
};


const uint8_t GCOL_OPTS[] = {
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00,
    0x00,
    0xFF,
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


//************** MSB OF MEMORY OCCUPIED BY SCREEN BUFFER     *****************

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
        0x7f,   // 0 Horizontal Total    =128
        0x50,   // 1 Horizontal Displayed =80
        0x62,   // 2 Horizontal Sync     =&62
        0x28,   // 3 HSync Width+VSync   =&28  VSync=2, HSync Width=8
        0x26,   // 4 Vertical Total  =38
        0x00,   // 5 Vertial Adjust  =0
        0x20,   // 6 Vertical Displayed  =32
        0x22,   // 7 VSync Position  =&22
        0x01,   // 8 Interlace+Cursor    =&01  Cursor=0, Display=0, Interlace=Sync
        0x07,   // 9 Scan Lines/Character =8
        0x67,   // 10 Cursor Start Line   =&67  Blink=On, Speed=1/32, Line=7
        0x08    // 11 Cursor End Line     =8
    },
    {
        // 6845 REGISTERS 0-11 FOR SCREEN TYPE 1 - MODE 3
        0x7f,   // 0 Horizontal Total    =128
        0x50,   // 1 Horizontal Displayed =80
        0x62,   // 2 Horizontal Sync     =&62
        0x28,   // 3 HSync Width+VSync   =&28  VSync=2, HSync=8
        0x1e,   // 4 Vertical Total  =30
        0x02,   // 5 Vertical Adjust     =2
        0x19,   // 6 Vertical Displayed  =25
        0x1b,   // 7 VSync Position  =&1B
        0x01,   // 8 Interlace+Cursor    =&01  Cursor=0, Display=0, Interlace=Sync
        0x09,   // 9 Scan Lines/Character =10
        0x67,   // 10 Cursor Start Line   =&67  Blink=On, Speed=1/32, Line=7
        0x09    // 11 Cursor End Line     =9
    }, {
        // 6845 REGISTERS 0-11 FOR SCREEN TYPE 2 - MODES 4-5

        0x3f,   // 0 Horizontal Total    =64
        0x28,   // 1 Horizontal Displayed =40
        0x31,   // 2 Horizontal Sync     =&31
        0x24,   // 3 HSync Width+VSync   =&24  VSync=2, HSync=4
        0x26,   // 4 Vertical Total  =38
        0x00,   // 5 Vertical Adjust     =0
        0x20,   // 6 Vertical Displayed  =32
        0x22,   // 7 VSync Position  =&22
        0x01,   // 8 Interlace+Cursor    =&01  Cursor=0, Display=0, Interlace=Sync
        0x07,   // 9 Scan Lines/Character =8
        0x67,   // 10 Cursor Start Line   =&67  Blink=On, Speed=1/32, Line=7
        0x08    // 11 Cursor End Line     =8
    }, {
        // 6845 REGISTERS 0-11 FOR SCREEN TYPE 3 - MODE 6
        0x3f,   // 0 Horizontal Total    =64
        0x28,   // 1 Horizontal Displayed =40
        0x31,   // 2 Horizontal Sync     =&31
        0x24,   // 3 HSync Width+VSync   =&24  VSync=2, HSync=4
        0x1e,   // 4 Vertical Total  =30
        0x02,   // 5 Vertical Adjust     =0
        0x19,   // 6 Vertical Displayed  =25
        0x1b,   // 7 VSync Position  =&1B
        0x01,   // 8 Interlace+Cursor    =&01  Cursor=0, Display=0, Interlace=Sync
        0x09,   // 9 Scan Lines/Character =10
        0x67,   // 10 Cursor Start Line   =&67  Blink=On, Speed=1/32, Line=7
        0x09    // 11 Cursor End Line     =9
    }, {

        // 6845 REGISTERS 0-11 FOR SCREEN TYPE 4 - MODE 7 
        0x3f,   // 0 Horizontal Total    =64
        0x28,   // 1 Horizontal Displayed =40
        0x33,   // 2 Horizontal Sync     =&33  Note: &31 is a better value
        0x24,   // 3 HSync Width+VSync   =&24  VSync=2, HSync=4
        0x1e,   // 4 Vertical Total  =30
        0x02,   // 5 Vertical Adjust     =2
        0x19,   // 6 Vertical Displayed  =25
        0x1b,   // 7 VSync Position  =&1B
        0x93,   // 8 Interlace+Cursor    =&93  Cursor=2, Display=1, Interlace=Sync+Video
        0x12,   // 9 Scan Lines/Character =19
        0x72,   // 10 Cursor Start Line   =&72  Blink=On, Speed=1/32, Line=18
        0x13    // 11 Cursor End Line     =19
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


//*********** TELETEXT CHARACTER CONVERSION TABLE  ************************

uint8_t _TELETEXT_CHAR_TAB[] = {

    0x23,               // '#' -> '_'
    0x5f,               // '_' -> '`'
    0x60,               // '`' -> '#'
    0x23                // '#'
};


//****** 16 COLOUR MODE BYTE MASK LOOK UP TABLE******
// also used to translate a nybble of font def to 4 colour pixel values

uint8_t _COL16_MASK_TAB[] = {
    0x00,
    0x11,
    0x22,
    0x33,
    0x44,
    0x55,
    0x66,
    0x77,
    0x88,
    0x99,
    0xaa,
    0xbb,
    0xcc,
    0xdd,
    0xee,
    0xff
};

//****** 4 COLOUR MODE BYTE MASK LOOK UP TABLE******
// also used to translate a pair of bits in a font definition to MODE 2 pixels

uint8_t _COL4_MASK_TAB[] = {
    0x00,
    0x55,
    0xaa,
    0xff
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

    n ^= 7;     //convert to ULA colour numbers
    OSB_VIDPROC_PAL = n;        // pointless?
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
 *                                                                       *
 *   VDU 19 - DEFINE COLOURS                                             *
 *  [COLOUR l,p]                                                         *
 *  [COLOUR l,r,g,b]                                                     *
 *                                                                       *
 *   5 parameters                                                        *
 *                                                                       *
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
 *                                                                       *
 *   VDU 20 - RESTORE DEFAULT COLOURS                                    *
 *                                                                       *
 *************************************************************************/

void VDU_20(void) {
    //TODO: look at reduce size?
    VDU_T_FG = 0;
    VDU_T_BG = 0;
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

uint16_t SET_CURS_CHARSCANAX(uint16_t addr) {
    VDU_TOP_SCAN = addr;    
    return SET_CRTC_16DIV8(0xE, VDU_CRTC_CUR);
}


uint16_t SET_CRTC_CURS16_adj(uint16_t addr) {
    VDU_CRTC_CUR = addr;
    if (addr & 0x8000)
        addr -= VDU_MEM_PAGES << 8;
    return SET_CURS_CHARSCANAX(addr);
}

int16_t _LD176_calcintcoord(bool rel, int16_t coord, bool isY) {

    int16_t n = (rel)?coord:coord+((isY)?VDU_G_CUR_EXT.y:VDU_G_CUR_EXT.x);
    if (isY)
    {
        VDU_G_CUR_EXT.y = n;
        n += VDU_G_ORG_EXT.y;
    } else  {
        VDU_G_CUR_EXT.x = n;
        n += VDU_G_ORG_EXT.x;
    }

    return n / 2;

}

vdu_coord16 _LD149_scale_coord_mode(bool rel, vdu_coord16 coord) {
    vdu_coord16 ret;
    ret.y = _LD176_calcintcoord(rel, coord.y, true) / 2;
    int d;
    if (VDU_PIX_BYTE > 3)
        d = 1;
    else if (VDU_PIX_BYTE == 3)
        d = 2;
    else
        d = 4;
    if (VDU_MAP_TYPE)
        d = d * 2;
    ret.x = _LD176_calcintcoord(rel, coord.x, false) / d;

    return ret;
}


/*************************************************************************
 *                                                                       *
 *   VDU 24 - DEFINE GRAPHICS WINDOW                                     *
 *                                                                       *
 *   8 parameters                                                        *
 *                                                                       *
 *************************************************************************
 &31C/D Left margin
 &31E/F Bottom margin
 &320/1 Right margin
 &322/3 Top margin
*/
void VDU_24(void) {
    vdu_coord16 oldcur = VDU_G_CUR_EXT;
        
    assert(!(((int)VDU_QUEUE + 1) & 1)); // must be halfword aligned
    vdu_box16 new = *((vdu_box16 *)(VDU_QUEUE+1));

    if (
        (new.topright.x - new.bottomleft.x < 0 )
    ||  (new.topright.y - new.bottomleft.y < 0 )
    ) goto abandon;

    new.bottomleft = _LD149_scale_coord_mode(false, new.bottomleft);
    new.topright = _LD149_scale_coord_mode(false, new.topright);
    
    if (new.bottomleft.x < 0 || new.bottomleft.y <0)
        goto abandon;
    if (new.topright.y >= 256)
        goto abandon;
    int16_t xx = new.topright.x / 8;

    if (xx > _TEXT_COL_TABLE[VDU_MODE])
        goto abandon;

    VDU_G_WIN = new;
    return;

abandon:
    VDU_G_CUR_EXT = oldcur;
    return;

}

uint16_t _LCF06_calc_text_scan() {
    //TODO: check - this is greatly simplified...
    uint16_t a = VDU_MEM + (VDU_BPR * VDU_T_CURS_Y) + (VDU_BPC * VDU_T_CURS_X);

    VDU_CRTC_CUR = a;
    if (a & 0x8000)
        a = a - (VDU_MEM_PAGES << 8);

    VDU_TOP_SCAN = a;
    return a;
}


/************:set up display address *************************************

;Mode 0: (0319)*640+(0318)* 8
;Mode 1: (0319)*640+(0318)*16
;Mode 2: (0319)*640+(0318)*32
;Mode 3: (0319)*640+(0318)* 8
;Mode 4: (0319)*320+(0318)* 8
;Mode 5: (0319)*320+(0318)*16
;Mode 6: (0319)*320+(0318)* 8
;Mode 7: (0319)* 40+(0318)
;this gives a displacement relative to the screen RAM start address
;which is added to the calculated number and stored in in 34A/B
;if the result is less than &8000, the top of screen RAM it is copied into X/A
;and D8/9.
;if the result is greater than &7FFF the hi byte of screen RAM size is
;subtracted to wraparound the screen. X/A, D8/9 are then set from this
*/

/********** move text cursor to next line **********************
; Cy set check against top
; returns true if caller should return immediately (on MOS it frigs the stack)
*/

bool _LCD3F(uint8_t flags) {
    if (!(VDU_STATUS & (VDUSTATUS_NO_SCROLL|VDUSTATUS_CURSOR_EDIT)))
        return false;
    uint8_t bound = (flags & FLAG_C)?VDU_T_WIN_T:VDU_T_WIN_B;
    if (!(VDU_STATUS & VDUSTATUS_CURSOR_EDIT)) {
        //not cursor editing wrap to other end of the screen
        VDU_T_CURS_Y = bound;
        SET_CURS_CHARSCANAX(_LCF06_calc_text_scan());           
        return true;    //indicate no further action
    } else {
        //cursor editing
        if (bound != VDU_TI_CURS_Y)
            VDU_TI_CURS_Y += (flags & FLAG_C)?-1:+1;
    }

    return false;
}

void _LCEAC_clear_line(void) {
    uint8_t xsave = VDU_T_CURS_X;
    VDU_T_CURS_X = VDU_T_WIN_L;
    _LCF06_calc_text_scan();
    for (int i = VDU_T_WIN_L; i <= VDU_T_WIN_R; i++) {
        memset(SCREENADDR(VDU_TOP_SCAN), VDU_T_BG, VDU_BPC);    //blank 1 char cell
        VDU_TOP_SCAN+=VDU_BPC;
        if (VDU_TOP_SCAN & 0x8000)
            VDU_TOP_SCAN -= (VDU_MEM_PAGES << 8);
    }
    VDU_T_CURS_X = xsave;
}

void _LCDFF_soft_scroll_up(void) {
    //TODO:_LCDFF_soft_scroll_up
    DEBUG_PRINT_STR("SOFT SCROLL UP");
}

void _LCDA4_soft_scroll_down(void) {
    //TODO:_LCDA4_soft_scroll_down
    DEBUG_PRINT_STR("SOFT SCROLL DOWN");
}

void _LC994_hard_scroll_down(void) {
    uint16_t m = VDU_MEM - VDU_BPR;
    if ((m >> 8) > VDU_PAGE)
        m += (VDU_MEM_PAGES << 8);
    VDU_MEM = m;
    SET_CRTC_16DIV8(0x0C, VDU_MEM);     
}

void _LC9A4_hard_Scroll_up(void) {
    uint16_t m = VDU_MEM + VDU_BPR;
    if (m & 0x8000)
        m = m - (VDU_MEM_PAGES << 8);
    VDU_MEM = m;
    SET_CRTC_16DIV8(0x0C, VDU_MEM);
}

void _BC68A_txt_cursor_down(void) {
    //TEXT CUROR DOWN
    //TODO: PAGED MODE
    if (VDU_T_CURS_Y < VDU_T_WIN_B)
    {
        VDU_T_CURS_Y++;
    } else {
        if (_LCD3F(FLAG_C)) return; // check for wrap round due to no scroll / cursor edit
        if (VDU_STATUS & VDUSTATUS_SOFT_SCROLL)
            _LCDFF_soft_scroll_up();    // soft scroll
        else
            _LC9A4_hard_Scroll_up();    // hard scroll
        _LCEAC_clear_line();            // clear a line
        
    }
    SET_CURS_CHARSCANAX(_LCF06_calc_text_scan());
    return;
}


/*************************************************************************
 *                                                                       *
 *   VDU 9  - CURSOR RIGHT                                               *
 *                                                                       *
 *************************************************************************/

void VDU_9(void) {
    //TODO: GRAPHICS
    if (VDU_T_CURS_X <= VDU_T_WIN_R) {
        VDU_T_CURS_X++;
        VDU_CRTC_CUR += VDU_BPC;
        SET_CRTC_CURS16_adj(VDU_CRTC_CUR);
        return;
    } else {
        VDU_T_CURS_X = VDU_T_WIN_L;
        _BC68A_txt_cursor_down();
        return;
    }
}


void _BC5F4_txt_cursor_up(void) {
    if (OSB_HALT_LINES > 0)
        OSB_HALT_LINES--;

    if (VDU_T_CURS_Y != VDU_T_WIN_T) {
        VDU_T_CURS_Y--;
    } else {
        _LCD3F(0);
        if (VDU_STATUS & VDUSTATUS_SOFT_SCROLL)
            _LCDA4_soft_scroll_down();
        else
            _LC994_hard_scroll_down();
        SET_CURS_CHARSCANAX(_LCF06_calc_text_scan());
    }
    SET_CURS_CHARSCANAX(_LCF06_calc_text_scan());
    return;
}

/*************************************************************************
 *                                                                       *
 *   VDU 10  - CURSOR DOWN                                               *
 *                                                                       *
 *************************************************************************/

void VDU_10(void) {
    if (!(VDU_STATUS & VDUSTATUS_GFX)) {
        _BC68A_txt_cursor_down();
        return;
    } else {
        //TODO: VDU5
    }   
}

/*************************************************************************
 *                                                                       *
 *   VDU 11  - CURSOR UP                                                 *
 *                                                                       *
 *************************************************************************/

void VDU_11(void) {
    if (!(VDU_STATUS & VDUSTATUS_GFX)) {
        _BC5F4_txt_cursor_up();
        return;
    } else {
        //TODO: VDU5
    }   
}


/*************************************************************************
 *                                                                       *
 *   VDU 8  - CURSOR LEFT                                                *
 *                                                                       *
 *************************************************************************/

void VDU_8(void) {
    if (!(VDU_STATUS & VDUSTATUS_GFX)) {
        int X = VDU_T_CURS_X - 1;
        if (X < 0) {
            VDU_T_CURS_X = VDU_T_WIN_R;
            _BC5F4_txt_cursor_up();
            return;
        } else {
            VDU_CRTC_CUR -= VDU_BPC;
            if (VDU_CRTC_CUR < (VDU_PAGE << 8)) {
                VDU_CRTC_CUR += VDU_MEM_PAGES << 8;
            }
            VDU_T_CURS_X = X;
            SET_CRTC_CURS16_adj(VDU_CRTC_CUR);
            return;
        }
    } else {
        //TODO: VDU5
    }
}

uint8_t _LD10F_checkGWINLIMS(vdu_coord16 coord) {
    uint8_t ret = 0;
    if (coord.y < VDU_G_WIN.bottomleft.y)
        ret |= 4;
    else if (coord.y > VDU_G_WIN.topright.y)
        ret |= 8;

    if (coord.x < VDU_G_WIN.bottomleft.x)
        ret |= 1;
    else if (coord.x > VDU_G_WIN.topright.x)
        ret |= 2;

    return ret;
}

void _LD0D9_MOVEGCURS(vdu_coord16 coord) {
    VDU_G_CURPREV_INT = VDU_G_CURS;
    VDU_G_CURS = coord;
}

uint8_t _LD85D_calcpoint(vdu_coord16 coord) {
    uint8_t r = _LD10F_checkGWINLIMS(coord);
    if (r)
        return r;       //out of bounds - exit

    VDU_G_CURS_SCAN = (coord.y & 0x7) ^ 7;  // row within cell

    VDU_G_MEM = VDU_MEM + ((coord.y ^ 0xFF) >> 3) * VDU_BPR;

    VDU_G_PIX_MASK = VDU_MASK_LEFT >> (coord.x & VDU_PIX_BYTE); // a bit mask

    //TODO: speed up with shifts instead of divs?
    VDU_G_MEM = VDU_G_MEM + 8*(coord.x / (1 + VDU_PIX_BYTE));

    if (VDU_G_MEM & 0x8000)
        VDU_G_MEM -= (VDU_MEM_PAGES << 8);

    return 0;
}

void point_at_G_MEM(void) {

    uint8_t *p = (uint8_t *)(0xFFFF0000 + (VDU_G_CURS_SCAN + VDU_G_MEM));

    uint8_t t1 = (VDU_G_PIX_MASK & VDU_G_OR_MASK) | *p;

    *p = (VDU_G_EOR_MASK & VDU_G_PIX_MASK) ^ t1;

}

void _LD0F0_plot_int(vdu_coord16 coord) {
    if (_LD85D_calcpoint(coord))
        return;
    point_at_G_MEM();
}

void _LD0B3_set_colour_masks(uint8_t c, uint8_t m) {

    VDU_G_OR_MASK = (c | GCOL_OPTS[1+m]) ^ GCOL_OPTS[2+m];
    VDU_G_EOR_MASK = (c | GCOL_OPTS[0+m]) ^ GCOL_OPTS[5+m];

}

vdu_coord16 subtract_coords(vdu_coord16 a, vdu_coord16 b) {
    vdu_coord16 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    return r;
}

void _LD3ED_drawline_right_cell(void) {
    VDU_G_PIX_MASK = VDU_MASK_LEFT;
    VDU_G_MEM += 8;
    if (VDU_G_MEM & 0x8000)
        VDU_G_MEM -= (VDU_MEM_PAGES << 8);
}

void _LD3FD_drawline_left_cell(void) {
    VDU_G_PIX_MASK = VDU_MASK_RIGHT;
    VDU_G_MEM -= 8;
    if (VDU_G_MEM < (VDU_PAGE << 8))
        VDU_G_MEM += (VDU_MEM_PAGES << 8);
}

void _LD3D3_drawline_upone_cell(void) {
    VDU_G_MEM -= VDU_BPR;
    if (VDU_G_MEM < (VDU_PAGE << 8))
        VDU_G_MEM += (VDU_MEM_PAGES << 8);
    VDU_G_CURS_SCAN = 7;
}

void _drawline_downone_cell(void) {
    VDU_G_MEM += VDU_BPR;
    if (VDU_G_MEM & 0x8000)
        VDU_G_MEM -= (VDU_MEM_PAGES << 8);
    VDU_G_CURS_SCAN = 0;
}

void _LD1ED_draw_line(vdu_coord16 coord) {

    vdu_coord16 sz = subtract_coords(VDU_G_CURS, coord);

    int slope;
    int16_t endmaj;
    vdu_coord16 xy;
    vdu_coord16 end;
    uint8_t flags = (VDU_QUEUE[4] & 0x10) << 3;     //set dot flag if requested in bit 7
    bool flip = false;
    if (abs(sz.y) > abs(sz.x))
    {
        slope = 2;
        flip = sz.y > 0;
    } else {
        slope = 0;
        flip = sz.x > 0;
    }

    if (flip) {
        xy = coord;
        end = VDU_G_CURS;
    } else {
        xy = VDU_G_CURS;
        end = coord;
    }
    endmaj = (slope)?end.y:end.x;

    uint8_t limstart = _LD10F_checkGWINLIMS(xy);
    if (limstart)
        flags |= 0x40;
    uint8_t limsend = _LD10F_checkGWINLIMS(end);
    if (limstart & limsend)
    {
        return;                 // both ends out of bounds in same dimension must be off screen
    }

    if (slope)
        limstart >>= 2;
    if (limstart & 2) {
        endmaj = (slope)?VDU_G_WIN.topright.y:VDU_G_WIN.topright.x;
    }

    uint16_t delta_minor;
    uint16_t delta_major;
    int32_t  error_acc;
    int loopcounter = -abs(endmaj - ((slope)?xy.y:xy.x));

    //limits all worked out now initialise the Bresenham deltas and accumulator
    if (slope) {
        delta_major = abs(sz.y);
        delta_minor = abs(sz.x);
    } else {
        delta_major = abs(sz.x);
        delta_minor = abs(sz.y);        
    }
    error_acc = delta_major >> 1;

    //now work out which minor function we need
    uint8_t slope2 = slope ^ 2;
    if ((sz.x < 0) != (sz.y < 0)) {
        // if X/Y slopes different
        slope2++;
    }


    bool dotornot = true;
    uint8_t minorcounter;
    
    switch (slope) {
        case 0:
            //right
            minorcounter = VDU_G_WIN.topright.x - xy.x;
            break;
        case 1:
            //left
            minorcounter = VDU_G_WIN.bottomleft.x - xy.x;
            break;
        case 2:
            //up
            minorcounter = VDU_G_WIN.topright.y - xy.y;
            break;
        case 3:
            //down
            minorcounter = VDU_G_WIN.bottomleft.y - xy.y;
            break;
    }
    minorcounter = abs(minorcounter)+1;

    _LD85D_calcpoint(xy);

    loopcounter--;


//    DEBUG_PRINTF("W:%d,H:%d\n,X:%d,Y:%d\nEMAJ:%d,S:%d,S2:%d,F:%x,LC:%d,LC2:%d\n", (int)sz.x, (int)sz.y, (int)xy.x, (int)xy.y, (int)endmaj, (int)slope, (int)slope2, (int)flags, (int)loopcounter, (int)minorcounter);


    do {        
        bool dopoint = false;
        if (flags & 0x80) {
            dopoint = dotornot;
            dotornot = !dotornot;
        } else if (flags == 0) {
            dopoint = true;
        } else {
            //OOB: check 
            dopoint = _LD85D_calcpoint(xy) == 0;
        }


        if (dopoint)
        {
            point_at_G_MEM();
        }
        
        error_acc -= delta_minor;
        if (error_acc < 0) {
            if (((flags & 0x40) == 0) && !(--minorcounter))
            {
                return;
            }

            error_acc += delta_major;
            
            switch (slope2) {
                case 0: // right
                    if (VDU_G_PIX_MASK & 0x01)
                        _LD3ED_drawline_right_cell();
                    else
                        VDU_G_PIX_MASK >>= 1;
                    if (flags & 0x40)
                        xy.x++;
                    break;
                case 1: // left
                    if (VDU_G_PIX_MASK & 0x80)
                        _LD3FD_drawline_left_cell();
                    else
                        VDU_G_PIX_MASK <<= 1;
                    if (flags & 0x40)
                        xy.x--;
                    break;
                case 2: // up
                    if (!VDU_G_CURS_SCAN)
                        _LD3D3_drawline_upone_cell();
                    else
                        VDU_G_CURS_SCAN--;
                    if (flags & 0x40)   
                        xy.y++;
                    break;
                case 3: // down
                    if (VDU_G_CURS_SCAN == 7)
                        _drawline_downone_cell();
                    else
                        VDU_G_CURS_SCAN++;
                    if (flags & 0x40)   
                        xy.y--;
                    break;
            }
        }
        //major
        if (slope) {
            if (!VDU_G_CURS_SCAN)
                _LD3D3_drawline_upone_cell();
            else
                VDU_G_CURS_SCAN--;
            if (flags & 0x40)   
                xy.y++;
        } else {
            if (VDU_G_PIX_MASK & 0x01)
                _LD3ED_drawline_right_cell();
            else
                VDU_G_PIX_MASK >>= 1;
            if (flags & 0x40)
                xy.x++;
        }
        
        if (!++loopcounter)
        {
            return;
        }


    } while (1);

}

/*************************************************************************
 *                                                                       *
 *   VDU 25 - PLOT                                                       *
 *   PLOT k,x,y                                                          *
 *   DRAW x,y                                                            *
 *   MOVE x,y                                                            *
 *   PLOT x,y                                                            *
 *   5 parameters                                                        *
 *                                                                       *
 *************************************************************************/

void VDU_25(void) {
    uint8_t n = VDU_QUEUE[4];
    if (!VDU_PIX_BYTE) {
        //text mode
        VDUV(false, n);
        return;
    } else {
        
//        DEBUG_PRINTF("VDU25,%x,%d,%d\n", (int)n, (int)*((int16_t *)(VDU_QUEUE+5)),(int)*((int16_t *)(VDU_QUEUE+7)));

        assert(!(((int)VDU_QUEUE + 5) & 1)); // must be halfword aligned
        vdu_coord16 coord = *((vdu_coord16 *)(VDU_QUEUE+5));

        coord = _LD149_scale_coord_mode(n & 4, coord);
        
        if (n == 0x04) {
            // quick move
            _LD0D9_MOVEGCURS(coord);
            return;
        } else {
            if (!(n & 3))
                _LD0B3_set_colour_masks(0, 5);
            else if (n & 1)
                _LD0B3_set_colour_masks((n&2)?VDU_G_BG:VDU_G_FG, (n&2)?VDU_P_BG:VDU_P_FG);
            else
                _LD0B3_set_colour_masks(0, 4);
        }
        if (n & 0x80) {
            //extension
            VDUV(false, n);
            return;
        } else if (n < 32) {
            if (n & 8)
                _LD0F0_plot_int(coord);  //extra point plot at end of line for inverts
            _LD1ED_draw_line(coord);
            _LD0D9_MOVEGCURS(coord);
        } else if (n < 64) {
            //extension 32..63
            VDUV(false, n);
            return;
        } else if (n >=64 && n < 72) {
            //TODO: other types
            _LD0F0_plot_int(coord);
            _LD0D9_MOVEGCURS(coord);
        } else {
            //TODO: GFX: fills/triangles
        }
    }
}

//***** calculate external coordinates from internal coordinates************

void _LD1B8_calc_ext(void) {
    VDU_G_CUR_EXT.y = (VDU_G_CURS.y << 2) - VDU_G_ORG_EXT.y;
    int n = VDU_PIX_BYTE;
    int s = 4;
    do {
        s--;
        n >>= 1;
    } while (n);
    VDU_G_CUR_EXT.x = (VDU_G_CURS.x << s) - VDU_G_ORG_EXT.x;
}

/*************************************************************************
 *                                                                       *
 *   VDU 26 - SET DEFAULT WINDOWS                                        *
 *                                                                       *
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

    memset(SCREENADDR(VDU_MEM), VDU_T_BG, (uint16_t)VDU_MEM_PAGES << 8);

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

void VDU_22(void) {
    vdu_mode(VDU_QUEUE[8]);
}

void VDU_0(void) {

}

// returns true if outside window
bool _LCEE8_check_text_cursor_bounds(void) {
    if (VDU_T_CURS_X >= VDU_T_WIN_L 
    && VDU_T_CURS_X <= VDU_T_WIN_R
    && VDU_T_CURS_Y >= VDU_T_WIN_T
    && VDU_T_CURS_Y <= VDU_T_WIN_B)
    {
        _LCF06_calc_text_scan();
        return 0;
    }
    else
        return 1;
}


/*************************************************************************
 *                                                                       *
 *   VDU 31 - POSITION TEXT CURSOR                                       *
 *   TAB(X,Y)                                                            *
 *                                                                       *
 *   2 parameters                                                        *
 *                                                                       *
 *************************************************************************
 0322 = supplied X coordinate
 0323 = supplied Y coordinate
*/
void VDU_31(void) {
    if (VDU_STATUS & VDUSTATUS_GFX)
    {
        //TODO: VDU5
    } else {
        int8_t sx = VDU_T_CURS_X;
        int8_t sy = VDU_T_CURS_X;
        VDU_T_CURS_X = (int8_t)VDU_QUEUE[7] + VDU_T_WIN_L;
        VDU_T_CURS_Y = (int8_t)VDU_QUEUE[8] + VDU_T_WIN_T;
        if (!_LCEE8_check_text_cursor_bounds()) {
            SET_CURS_CHARSCANAX(VDU_TOP_SCAN);              
        } else {
            VDU_T_CURS_X = sx;
            VDU_T_CURS_Y = sy;
        }

    }
}


/*************************************************************************
 *                                                                       *
 *   VDU 30 - HOME CURSOR                                                *
 *                                                                       *
 *************************************************************************/

void VDU_30(void) {
    if (VDU_STATUS & VDUSTATUS_GFX)
    {
        //TODO: VDU5
    } else {
        VDU_QUEUE[7] = 0;
        VDU_QUEUE[8] = 0;
        VDU_31();
    }
}

/*************************************************************************
 *                                                                       *
 *   VDU 12 - CLEAR TEXT SCREEN                                          *
 *   CLS                                                                 *
 *                                                                       *
 *************************************************************************/
void VDU_12(void) {
    if (VDU_STATUS & VDUSTATUS_GFX)
    {
        //TODO: VDU5
    } else {
        if (VDU_STATUS & VDUSTATUS_SOFT_SCROLL) {
            for (VDU_T_CURS_Y=VDU_T_WIN_T; VDU_T_CURS_Y <= VDU_T_WIN_B; VDU_T_CURS_Y++)
                _LCEAC_clear_line();
            VDU_30();       // home
        } else 
            _LCBC1_DOCLS();
    }
}

/*************************************************************************
 *                                                                       *
 *   VDU 13 - CARRIAGE RETURN                                            *
 *                                                                       *
 *************************************************************************/
void VDU_13(void) {
    if (VDU_STATUS & VDUSTATUS_GFX)
    {
        //TODO: VDU5
    } else {
        VDU_T_CURS_X = VDU_T_WIN_L;
        SET_CURS_CHARSCANAX(_LCF06_calc_text_scan());
    }
}

void _BC7FF(uint8_t Y) {
    uint8_t c = VDU_QUEUE[8];
    if (c & 0x80)
        Y++;;
    c = c & VDU_COL_MASK;
    if (!VDU_COL_MASK)
        return;
    uint8_t X = (VDU_COL_MASK & 7) + c - 1;
    uint8_t f = _LC432_COLOURS_TAB[X];
    ((uint8_t *)&VDU_T_FG)[Y] = f;
    if (Y>=2) {
        // graphics mode
        ((uint8_t *)&VDU_G_FG)[Y] = VDU_QUEUE[7]; // GCOL code
    } else {
        VDU_T_EOR_MASK = VDU_T_FG ^ 0xFF;
        VDU_T_OR_MASK = VDU_T_EOR_MASK ^ VDU_T_BG;      
    }
}

/*************************************************************************
 *                                                                       *
 *   VDU 17 - DEFINE TEXT COLOUR                                         *
 *   COLOUR n                                                            *
 *                                                                       *
 *   1 parameter                                                         *
 *                                                                       *
 *************************************************************************/

void VDU_17(void) {
    _BC7FF(0);
}

/*************************************************************************
 *                                                                       *
 *   VDU 18 - DEFINE GRAPHICS COLOUR                                     *
 *   GCOL k,c                                                            *
 *                                                                       *
 *   2 parameters                                                        *
 *                                                                       *
 *************************************************************************/

void VDU_18(void) {
    _BC7FF(2);
}


/*************************************************************************
 *                                                                       *
 *   VDU 29 - SET GRAPHICS ORIGIN                                        *
 *                                                                       *
 *   4 parameters                                                        *
 *                                                                       *
 *************************************************************************/

void VDU_29(void) {
    VDU_G_ORG_EXT = *((vdu_coord16 *)(VDU_QUEUE+5));
    _LD1B8_calc_ext();
}


const uint8_t *_LD03E_font_addr(uint8_t c) {
    uint8_t r = c >> 6;     // usrdef region index  
    if (VDU_FONT_FLAGS & (0x80 >> r))
        return (uint8_t *) ((c & 0xF8) + (VDU_FONT_LOC[r-1] << 8));
    else
        return mos_FONT + (c-32) * 8;
}

void VDU_OUT_CHAR(uint8_t c) {
    if (VDU_COL_MASK == 0) {
        //MODE 7
        int Y = 2;
        do {
            if (_TELETEXT_CHAR_TAB[Y] == c) {
                c = _TELETEXT_CHAR_TAB[Y+1];
            }
            Y--;
        } while (Y >= 0);
        *SCREENADDR(VDU_TOP_SCAN) = c;
        return;
    } else {
        const uint8_t *fa = _LD03E_font_addr(c);
        if (VDU_STATUS & VDUSTATUS_GFX)
        {
            //TODO: VDU5
        } else {
            if (VDU_COL_MASK == 1)
                memcpy(SCREENADDR(VDU_TOP_SCAN), fa, 8);
            else if (VDU_COL_MASK == 3) {
                // mode 1/5
                for (int i = 0; i < 8; i++) {
                    uint8_t d = fa[i];
                    *SCREENADDR(VDU_TOP_SCAN+i) = 
                        (_COL16_MASK_TAB[d >> 4] | VDU_T_OR_MASK) ^ VDU_T_EOR_MASK;
                    *SCREENADDR(VDU_TOP_SCAN+i+8) = 
                        (_COL16_MASK_TAB[d & 0xF] | VDU_T_OR_MASK) ^ VDU_T_EOR_MASK;
                }
            } else {
                // mode 2
                for (int i = 0; i < 8; i++) {
                    uint8_t d = fa[i];
                    for (int j = 0; j < 4; j++) {
                        uint8_t d2 = d >> 6;
                        d = d << 2;
                        *SCREENADDR(VDU_TOP_SCAN+i+8*j) = 
                            (_COL4_MASK_TAB[d2] | VDU_T_OR_MASK) ^ VDU_T_EOR_MASK;
                    }
                }
            }
        }
    }
}

void vdu_init(uint8_t mode) {
    VDU_STATUS = 0;
    memset(&VDU_VARS_BASE, 0, VDU_VARS_SIZE);
    vdu_osbyte_20_EXPLODE();
    VDU_TTX_CURSOR = 0x7F;
    vdu_mode(mode);
}

typedef void (*VDU_FN)(void);

const VDU_FN _TBL_VDU_ROUTINES[33] = {
    VDU_0,      // VDU  0   - &C511, no parameters
    VDU_0,      // VDU  1   - &C53B, 1 parameter
    VDU_0,      // VDU  2   - &C596, no parameters
    VDU_0,      // VDU  3   - &C5A1, no parameters
    VDU_0,      // VDU  4   - &C5AD, no parameters
    VDU_0,      // VDU  5   - &C5B9, no parameters
    VDU_0,      // VDU  6   - &C511, no parameters
    VDU_0,      // VDU  7   - &E86F, no parameters
    VDU_8,      // VDU  8   - &C5C5, no parameters
    VDU_9,      // VDU  9   - &C664, no parameters
    VDU_10,     // VDU 10  - &C6F0, no parameters
    VDU_11,     // VDU 11  - &C65B, no parameters
    VDU_12,     // VDU 12  - &C759, no parameters
    VDU_13,     // VDU 13  - &C7AF, no parameters
    VDU_0,      // VDU 14  - &C58D, no parameters
    VDU_0,      // VDU 15  - &C5A6, no parameters
    VDU_0,      // VDU 16  - &C7C0, no parameters
    VDU_17,     // VDU 17  - &C7F9, 1 parameter
    VDU_18,     // VDU 18  - &C7FD, 2 parameters
    VDU_19,     // VDU 19  - &C892, 5 parameters
    VDU_20,     // VDU 20  - &C839, no parameters
    VDU_0,      // VDU 21  - &C59B, no parameters
    VDU_22,     // VDU 22  - &C8EB, 1 parameter
    VDU_0,      // VDU 23  - &C8F1, 9 parameters
    VDU_24,     // VDU 24  - &CA39, 8 parameters
    VDU_25,     // VDU 25  - &C9AC, 5 parameters
    VDU_26,     // VDU 26  - &C9BD, no parameters
    VDU_0,      // VDU 27  - &C511, no parameters
    VDU_0,      // VDU 28  - &C6FA, 4 parameters
    VDU_29,     // VDU 29  - &CAA2, 4 parameters
    VDU_30,     // VDU 30  - &C779, no parameters
    VDU_31,     // VDU 31  - &C787, 2 parameters
    VDU_0       // VDU 127 - &CAAC, no parameters
};

const int8_t _TBL_VDU_PARAMS[33] = {
    0,      // VDU  0   - &C511, no parameters
    -1,     // VDU  1   - &C53B, 1 parameter
    0,      // VDU  2   - &C596, no parameters
    0,      // VDU  3   - &C5A1, no parameters
    0,      // VDU  4   - &C5AD, no parameters
    0,      // VDU  5   - &C5B9, no parameters
    0,      // VDU  6   - &C511, no parameters
    0,      // VDU  7   - &E86F, no parameters
    0,      // VDU  8   - &C5C5, no parameters
    0,      // VDU  9   - &C664, no parameters
    0,      // VDU 10  - &C6F0, no parameters
    0,      // VDU 11  - &C65B, no parameters
    0,      // VDU 12  - &C759, no parameters
    0,      // VDU 13  - &C7AF, no parameters
    0,      // VDU 14  - &C58D, no parameters
    0,      // VDU 15  - &C5A6, no parameters
    0,      // VDU 16  - &C7C0, no parameters
    -1,     // VDU 17  - &C7F9, 1 parameter
    -2,     // VDU 18  - &C7FD, 2 parameters
    -5,     // VDU 19  - &C892, 5 parameters
    0,      // VDU 20  - &C839, no parameters
    0,      // VDU 21  - &C59B, no parameters
    -1,     // VDU 22  - &C8EB, 1 parameter
    -9,     // VDU 23  - &C8F1, 9 parameters
    -8,     // VDU 24  - &CA39, 8 parameters
    -5,     // VDU 25  - &C9AC, 5 parameters
    0,      // VDU 26  - &C9BD, no parameters
    0,      // VDU 27  - &C511, no parameters
    -4,     // VDU 28  - &C6FA, 4 parameters
    -4,     // VDU 29  - &CAA2, 4 parameters
    0,      // VDU 30  - &C779, no parameters
    -2,     // VDU 31  - &C787, 2 parameters
    0       // VDU 127 - &CAAC, no parameters
};

//TODO: these are hidden - move into header?
VDU_FN VDU_EXEC;

//TODO: in MOS returns CLC/SEC?
//TODO:PRINTER
//TODO:CURSOR
//TODO:status / disabled etc
void vdu_write(uint8_t c) {
    if (OSB_VDU_QSIZE) {
        VDU_QUEUE[9+OSB_VDU_QSIZE] = c;
        if (!++OSB_VDU_QSIZE)   
            VDU_EXEC();
    } else {
        if (c == 0x7F || c < 0x20) {
            if (c == 0x7F)
                c = 0x20;
            //TODO: CURSOR EDITING
            VDU_EXEC = _TBL_VDU_ROUTINES[c];
            OSB_VDU_QSIZE = _TBL_VDU_PARAMS[c];
            if (OSB_VDU_QSIZE == 0)
                VDU_EXEC();
        } else {
            //TODO: DELETE
            VDU_OUT_CHAR(c);
            VDU_9();
        }
    }
}

void vdu_default_VDUV(bool vdu23, uint8_t n) {

}
