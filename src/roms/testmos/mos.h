#ifndef __MOS_H__
#define __MOS_H__

#include <stdint.h>

// RISC-V Co Processor Sys Call Numbers

//#define ECALL_BASE  0x00AC0000
#define ECALL_BASE  0x00000000

#define OS_QUIT     (ECALL_BASE +  0)
#define OS_CLI      (ECALL_BASE +  1)
#define OS_BYTE     (ECALL_BASE +  2)
#define OS_WORD     (ECALL_BASE +  3)
#define OS_WRCH     (ECALL_BASE +  4)
#define OS_NEWL     (ECALL_BASE +  5)   // Unused
#define OS_RDCH     (ECALL_BASE +  6)
#define OS_FILE     (ECALL_BASE +  7)
#define OS_ARGS     (ECALL_BASE +  8)
#define OS_BGET     (ECALL_BASE +  9)
#define OS_BPUT     (ECALL_BASE + 10)
#define OS_GBPB     (ECALL_BASE + 11)
#define OS_FIND     (ECALL_BASE + 12)
#define OS_SYS_CTRL (ECALL_BASE + 13)
#define OS_HANDLERS (ECALL_BASE + 14)
#define OS_ERROR    (ECALL_BASE + 15)   // Unused


struct mos_args {
	uint32_t	a0;
	uint32_t	a1;
	uint32_t	a2;
	uint32_t	a3;
	uint32_t	a4;
	uint32_t	a5;
	uint32_t	a6;
};

#define	CRFS_LOAD		((uint32_t *)0xb0)
#define	CRFS_EXEC		((uint16_t *)0xb4)
#define	CRFS_NEXT_BLK		((uint16_t *)0xb6)
#define	CRFS_ERR_PTR		((uint16_t *)0xb8)
#define	CRFS_BLOCK_FLAG		((uint8_t *)0xba)
#define	CRFS_OPTS		((uint8_t *)0xbb)
#define	CRFS_BLK_OFFSET		((uint8_t *)0xbc)
#define	CRFS_BLK_LAST		((uint8_t *)0xbd)
#define	CRFS_CRC_TMP		((uint16_t *)0xbe)
#define	CRFS_BUFFER_FLAG	((uint8_t *)0xc0)
#define	CRFS_CRC_RESULT		((uint8_t *)0xc1)
#define	CRFS_PROGRESS		((uint8_t *)0xc2)
#define	CFS_HANDLE		((uint8_t *)0xc3)
#define	CRFS_TMP		((uint8_t *)0xc4)
#define	CFS_BAUD_RATE		((uint8_t *)0xc6)
#define	CFS_INTERBLOCK		((uint8_t *)0xc7)
#define	CRFS_OSFILE_PTR		((uint16_t *)0xc8)
#define	CFS_SERIAL_CTRL		((uint8_t *)0xca)
#define	CRFS_CRC_BIT_CNT	((uint8_t *)0xcb)
#define	CRFS_FILE_SIZE		((uint16_t *)0xcc)

#define	VDU_STATUS		((uint8_t *)0xd0)
#define	VDU_G_PIX_MASK		((uint8_t *)0xd1)
#define	VDU_T_OR_MASK		((uint8_t *)0xd2)
#define	VDU_T_EOR_MASK		((uint8_t *)0xd3)
#define	VDU_G_OR_MASK		((uint8_t *)0xd4)
#define	VDU_G_EOR_MASK		((uint8_t *)0xd5)
#define	VDU_G_MEM		((uint16_t *)0xd6)
#define	VDU_TOP_SCAN		((uint16_t *)0xd8)
#define	VDU_TMP1		((uint8_t *)0xda)
#define	VDU_TMP2		((uint8_t *)0xdb)
#define	VDU_TMP3		((uint8_t *)0xdc)
#define	VDU_TMP4		((uint8_t *)0xdd)
#define	VDU_TMP5		((uint8_t *)0xde)
#define	VDU_TMP6		((uint8_t *)0xdf)
#define	VDU_ROW_MULT		((uint16_t *)0xe0)
#define	CRFS_STATUS		((uint8_t *)0xe2)
#define	CRFS_OPTIONS		((uint8_t *)0xe3)
#define	OSBYTE_PAR_3		((uint8_t *)0xe4)
#define	OSBYTE_PAR_2		((uint8_t *)0xe5)
#define	MOS_WS			((uint8_t *)0xe6)
#define	AUTO_REPEAT_TIMER	((uint8_t *)0xe7)
#define	OSW_0_PTR		((uint16_t *)0xe8)
#define	RS423_TIMEOUT		((uint8_t *)0xea)
#define	CRFS_ACTIVE		((uint8_t *)0xeb)
#define	KEYNUM_FIRST		((uint8_t *)0xec)
#define	KEYNUM_LAST		((uint8_t *)0xed)
#define	OSW_A			((uint8_t *)0xef)
#define	OSW_X			((uint8_t *)0xf0)
#define	OSW_Y			((uint8_t *)0xf1)
#define	TEXT_PTR		((uint16_t *)0xf2)
#define	ROM_SELECT		((uint8_t *)0xf4)
#define	RFS_SELECT		((uint8_t *)0xf5)
#define	ROM_PTR			((uint16_t *)0xf6)
#define	MOS_WS_0		((uint8_t *)0xfa)
#define	MOS_WS_1		((uint8_t *)0xfb)
#define	IRQ_COPY_A		((uint8_t *)0xfc)
#define	ERR_MSG_PTR		((uint16_t *)0xfd)
#define	ESCAPE_FLAG		((uint8_t *)0xff)



#define 	OSB_EXT_VEC		((uint16_t *)0x0238)
#define 	OSB_ROM_TABLE		((uint16_t *)0x023a)
#define 	OSB_KEY_TABLE		((uint16_t *)0x023c)
#define 	OSB_VDU_TABLE		((uint16_t *)0x023e)
#define 	OSB_CFS_TIMEOUT		((uint8_t *)0x0240)
#define 	OSB_IN_STREAM		((uint8_t *)0x0241)
#define 	OSB_KEY_SEM		((uint8_t *)0x0242)
#define 	OSB_OSHWM_DEF		((uint8_t *)0x0243)
#define 	OSB_OSHWM_CUR		((uint8_t *)0x0244)
#define 	OSB_RS423_MODE		((uint8_t *)0x0245)
#define 	OSB_CHAR_EXPL		((uint8_t *)0x0246)
#define 	OSB_CFSRFC_SW		((uint8_t *)0x0247)
#define 	OSB_VIDPROC_CTL		((uint8_t *)0x0248)
#define 	OSB_VIDPROC_PAL		((uint8_t *)0x0249)
#define 	OSB_LAST_ROM		((uint8_t *)0x024a)
#define 	OSB_BASIC_ROM		((uint8_t *)0x024b)
#define 	OSB_ADC_CHAN		((uint8_t *)0x024c)
#define 	OSB_ADC_MAX		((uint8_t *)0x024d)
#define 	OSB_ADC_ACC		((uint8_t *)0x024e)
#define 	OSB_RS423_USE		((uint8_t *)0x024f)
#define 	OSB_RS423_CTL		((uint8_t *)0x0250)
#define 	OSB_FLASH_TIME		((uint8_t *)0x0251)
#define 	OSB_FLASH_SPC		((uint8_t *)0x0252)
#define 	OSB_FLASH_MARK		((uint8_t *)0x0253)
#define 	OSB_KEY_DELAY		((uint8_t *)0x0254)
#define 	OSB_KEY_REPEAT		((uint8_t *)0x0255)
#define 	OSB_EXEC_HND		((uint8_t *)0x0256)
#define 	OSB_SPOOL_HND		((uint8_t *)0x0257)
#define 	OSB_ESC_BRK		((uint8_t *)0x0258)
#define 	OSB_KEY_DISABLE		((uint8_t *)0x0259)
#define 	OSB_KEY_STATUS		((uint8_t *)0x025a)
#define 	OSB_SER_BUF_EX		((uint8_t *)0x025b)
#define 	OSB_SER_BUF_SUP		((uint8_t *)0x025c)
#define 	OSB_SER_CAS_FLG		((uint8_t *)0x025d)
#define 	OSB_ECONET_INT		((uint8_t *)0x025e)
#define 	OSB_OSRDCH_INT		((uint8_t *)0x025f)
#define 	OSB_OSWRCH_INT		((uint8_t *)0x0260)
#define 	OSB_SPEECH_OFF		((uint8_t *)0x0261)
#define 	OSB_SOUND_OFF		((uint8_t *)0x0262)
#define 	OSB_BELL_CHAN		((uint8_t *)0x0263)
#define 	OSB_BELL_ENV		((uint8_t *)0x0264)
#define 	OSB_BELL_FREQ		((uint8_t *)0x0265)
#define 	OSB_BELL_LEN		((uint8_t *)0x0266)
#define 	OSB_BOOT_DISP		((uint8_t *)0x0267)
#define 	OSB_SOFT_KEYLEN		((uint8_t *)0x0268)
#define 	OSB_HALT_LINES		((uint8_t *)0x0269)
#define		OSB_VDU_QSIZE		((uint8_t *)0x026a)
#define		OSB_TAB			((uint8_t *)0x026b)
#define		OSB_ESCAPE		((uint8_t *)0x026c)
#define		OSB_CHAR_C0		((uint8_t *)0x026d)
#define		OSB_CHAR_D0		((uint8_t *)0x026e)
#define		OSB_CHAR_E0		((uint8_t *)0x026f)
#define		OSB_CHAR_F0		((uint8_t *)0x0270)
#define		OSB_CHAR_80		((uint8_t *)0x0271)
#define		OSB_CHAR_90		((uint8_t *)0x0272)
#define		OSB_CHAR_a0		((uint8_t *)0x0273)
#define		OSB_CHAR_b0		((uint8_t *)0x0274)
#define		OSB_ESC_ACTION		((uint8_t *)0x0275)
#define		OSB_ESC_EFFECTS		((uint8_t *)0x0276)
#define		OSB_UVIA_IRQ_M		((uint8_t *)0x0277)
#define		OSB_ACIA_IRQ_M		((uint8_t *)0x0278)
#define		OSB_SVIA_IRQ_M		((uint8_t *)0x0279)
#define		OSB_TUBE_FOUND		((uint8_t *)0x027a)
#define		OSB_SPCH_FOUND		((uint8_t *)0x027b)
#define		OSB_OUT_STREAM		((uint8_t *)0x027c)
#define		OSB_CURSOR_STAT		((uint8_t *)0x027d)
#define		OSB_KEYPAD_BASE		((uint8_t *)0x027e)
#define		OSB_SHADOW_RAM		((uint8_t *)0x027f)
#define		OSB_COUNTRY		((uint8_t *)0x0280)
#define		OSB_USER_FLAG		((uint8_t *)0x0281)
#define		OSB_SERPROC		((uint8_t *)0x0282)
#define		OSB_TIME_SWITCH		((uint8_t *)0x0283)
#define		OSB_SOFTKEY_FLG		((uint8_t *)0x0284)
#define		OSB_PRINT_DEST		((uint8_t *)0x0285)
#define		OSB_PRINT_IGN		((uint8_t *)0x0286)
#define		OSB_BRK_INT_JMP		((uint8_t *)0x0287)
#define		OSB_BRK_INT_LO		((uint8_t *)0x0288)
#define		OSB_BRK_INT_HI		((uint8_t *)0x0289)
#define		OSB_FA_UNUSED		((uint8_t *)0x028a)
#define		OSB_FB_UNUSED		((uint8_t *)0x028b)
#define		OSB_CUR_LANG		((uint8_t *)0x028c)
#define		OSB_LAST_BREAK		((uint8_t *)0x028d)
#define		OSB_RAM_PAGES		((uint8_t *)0x028e)
#define		OSB_STARTUP_OPT		((uint8_t *)0x028f)


#define		TIME_VAL1_MSB		((volatile uint8_t *)0x0292)
#define		TIME_VAL1_LSB		((volatile uint8_t *)0x0296)
#define		TIME_VAL2_MSB		((volatile uint8_t *)0x0297)
#define		TIME_VAL2_LSB		((volatile uint8_t *)0x029b)

#define		KEY_REPEAT_CNT		((uint8_t *)0x02ca)
#define		KEY_ROLLOVER_1		((uint8_t *)0x02cb)
#define		KEY_ROLLOVER_2		((uint8_t *)0x02cd)


#define FLAG_N 0x80
#define FLAG_V 0x40
#define FLAG_Z 0x02
#define FLAG_C 0x01

#define LATCH_0_SOUND 		0
#define LATCH_1_SPEECH_RS	1
#define LATCH_2_SPEECH_WS	2
#define LATCH_3_KEYSCAN		3
#define LATCH_4_C0		4
#define LATCH_5_C1		5
#define LATCH_6_LED_CAPS	6
#define LATCH_7_LED_SHIFT	7
#define LATCH_8_ON		8

#define KEYSTAT_SHIFT_EN	0x80
#define KEYSTAT_CTRL		0x40
#define KEYSTAT_SHIFT_LK	0x20
#define KEYSTAT_CAPS_LK		0x10
#define KEYSTAT_SHIFT		0x08

#define KEY_SHIFT 	0x00
#define KEY_CTRL  	0x01
#define KEY_SHIFT_LOCK 	0xD0
#define KEY_CAPS_LOCK 	0xC0

#endif