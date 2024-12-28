#ifndef __VDU_H__
#define __VDU_H__

#include <stdint.h>
#include <stdbool.h>

extern void vdu_init(uint8_t mode);
extern void vdu_write(uint8_t c);

extern void vdu_osbyte_20_EXPLODE(void);
extern void vdu_osbyte_154_SET_VID_ULA(uint8_t X);
extern void vdu_osbyte_155_SET_PALETTE(uint8_t X);

extern void vdu_default_VDUV(bool vdu23, uint8_t n);

extern void vidula_set(uint8_t n);

typedef struct {
	int16_t	x;
	int16_t y;
} vdu_coord16;

typedef struct {
	vdu_coord16 bottomleft;
	vdu_coord16 topright;
} vdu_box16;

#define VDU_VARS_BASE		(*((uint8_t *)0x0300))
#define VDU_VARS_SIZE		0x80

#define VDU_G_WIN		(*((vdu_box16 *)0x300))
#define VDU_G_WIN_L		(*((int16_t *)0x0300))
#define VDU_G_WIN_B		(*((int16_t *)0x0302))
#define VDU_G_WIN_R		(*((int16_t *)0x0304))
#define VDU_G_WIN_T		(*((int16_t *)0x0306))

#define VDU_T_WIN_L		(*((int8_t *)0x0308))
#define VDU_T_WIN_B		(*((int8_t *)0x0309))
#define VDU_T_WIN_R		(*((int8_t *)0x030a))
#define VDU_T_WIN_T		(*((int8_t *)0x030b))

#define VDU_G_ORG_EXT		(*((vdu_coord16 *)0x30c))
#define VDU_G_ORG_XX		(*((uint16_t *)0x030c))
#define VDU_G_ORG_YX		(*((uint16_t *)0x030e))

#define VDU_G_CUR_EXT		(*((vdu_coord16 *)0x310))
#define VDU_G_CURPREV_INT	(*((vdu_coord16 *)0x314))

#define VDU_T_CURS_X		(*((int8_t *)0x0318))
#define VDU_T_CURS_Y		(*((int8_t *)0x0319))
#define VDU_G_CURS_SCAN		(*((uint8_t *)0x031a))
#define VDU_QUEUE		((uint8_t *)0x031b)

#define VDU_G_CURS		(*((vdu_coord16 *)0x324))
#define VDU_BITMAP_READ		(*((uint8_t *)0x0328))

#define VDU_WORKSPACE		(*((void *)0x0330))

#define VDU_CRTC_CUR		(*((uint16_t *)0x034a))
#define VDU_T_WIN_SZ		(*((uint16_t *)0x034c))
#define VDU_PAGE		(*((uint8_t *)0x034e))
#define VDU_BPC			(*((uint8_t *)0x034f))

#define VDU_MEM			(*((uint16_t *)0x0350))
#define VDU_BPR			(*((uint16_t *)0x0352))
#define VDU_MEM_PAGES		(*((uint8_t *)0x0354))
#define VDU_MODE		(*((uint8_t *)0x0355))
#define VDU_MAP_TYPE		(*((uint8_t *)0x0356))
#define VDU_T_FG		(*((uint8_t *)0x0357))
#define VDU_T_BG		(*((uint8_t *)0x0358))
#define VDU_G_FG		(*((uint8_t *)0x0359))
#define VDU_G_BG		(*((uint8_t *)0x035a))
#define VDU_P_FG		(*((uint8_t *)0x035b))
#define VDU_P_BG		(*((uint8_t *)0x035c))
//#define VDU_JUMPVEC		(*((uint16_t *)0x035d)) - bad alignment and store in 32 bit somewhere else
#define VDU_CURS_PREV		(*((uint8_t *)0x035f))

#define VDU_COL_MASK		(*((uint8_t *)0x0360))
#define VDU_PIX_BYTE		(*((uint8_t *)0x0361))
#define VDU_MASK_LEFT		(*((uint8_t *)0x0362))
#define VDU_MASK_RIGHT		(*((uint8_t *)0x0363))
#define VDU_TI_CURS_X		(*((uint8_t *)0x0364))
#define VDU_TI_CURS_Y		(*((uint8_t *)0x0365))
#define VDU_TTX_CURSOR		(*((uint8_t *)0x0366))
#define VDU_FONT_FLAGS		(*((uint8_t *)0x0367))
#define VDU_FONT_LOC		((uint8_t *)0x0368)	//7 cell array
#define VDU_PALETTE		((uint8_t *)0x036f)





#define VDUSTATUS_PRINTER	0x01 		// VDU 2 printer output enabled
#define VDUSTATUS_NO_SCROLL	0x02		// Scrolling disabled (enabled by VDU 5 or editing cursor)
#define VDUSTATUS_PAGED		0x04		// VDU 14 paged mode selected
#define VDUSTATUS_SOFT_SCROLL	0x08		// Software scroll (set when window defined)
#define VDUSTATUS_SHADOW	0x10		// Master: Shadow screen mode, Electron: Soft cursor
#define VDUSTATUS_GFX		0x20		// VDU 5 enabled
#define VDUSTATUS_CURSOR_EDIT	0x40		// Editing cursor active
#define VDUSTATUS_DISABLE	0x80		// VDU 21 enabled, output disabled

#endif