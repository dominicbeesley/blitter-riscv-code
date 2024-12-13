#ifndef __HARDWARE_H__
#define __HARDWARE_H__

/***********************************************************************
 * System VIA                                                          *
 ***********************************************************************/
#define	sheila_SYSVIA_orb			((volatile uint8_t *)0xFFFFFE40)
#define	sheila_SYSVIA_ora			((volatile uint8_t *)0xFFFFFE41)
#define	sheila_SYSVIA_ddrb			((volatile uint8_t *)0xFFFFFE42)
#define	sheila_SYSVIA_ddra			((volatile uint8_t *)0xFFFFFE43)
#define	sheila_SYSVIA_t1cl			((volatile uint8_t *)0xFFFFFE44)
#define	sheila_SYSVIA_t1ch			((volatile uint8_t *)0xFFFFFE45)
#define	sheila_SYSVIA_t1ll			((volatile uint8_t *)0xFFFFFE46)
#define	sheila_SYSVIA_t1lh			((volatile uint8_t *)0xFFFFFE47)
#define	sheila_SYSVIA_t2cl			((volatile uint8_t *)0xFFFFFE48)
#define	sheila_SYSVIA_t2ch			((volatile uint8_t *)0xFFFFFE49)
#define	sheila_SYSVIA_sr			((volatile uint8_t *)0xFFFFFE4A)
#define	sheila_SYSVIA_acr			((volatile uint8_t *)0xFFFFFE4B)
#define	sheila_SYSVIA_pcr			((volatile uint8_t *)0xFFFFFE4C)
#define	sheila_SYSVIA_ifr			((volatile uint8_t *)0xFFFFFE4D)
#define	sheila_SYSVIA_ier			((volatile uint8_t *)0xFFFFFE4E)
#define	sheila_SYSVIA_ora_nh		((volatile uint8_t *)0xFFFFFE4F)

/***********************************************************************
 * User VIA                                                          *
 ***********************************************************************/
#define	sheila_USRVIA_orb			((volatile uint8_t *)0xFFFFFE60)
#define	sheila_USRVIA_ora			((volatile uint8_t *)0xFFFFFE61)
#define	sheila_USRVIA_ddrb			((volatile uint8_t *)0xFFFFFE62)
#define	sheila_USRVIA_ddra			((volatile uint8_t *)0xFFFFFE63)
#define	sheila_USRVIA_t1cl			((volatile uint8_t *)0xFFFFFE64)
#define	sheila_USRVIA_t1ch			((volatile uint8_t *)0xFFFFFE65)
#define	sheila_USRVIA_t1ll			((volatile uint8_t *)0xFFFFFE66)
#define	sheila_USRVIA_t1lh			((volatile uint8_t *)0xFFFFFE67)
#define	sheila_USRVIA_t2cl			((volatile uint8_t *)0xFFFFFE68)
#define	sheila_USRVIA_t2ch			((volatile uint8_t *)0xFFFFFE69)
#define	sheila_USRVIA_sr			((volatile uint8_t *)0xFFFFFE6A)
#define	sheila_USRVIA_acr			((volatile uint8_t *)0xFFFFFE6B)
#define	sheila_USRVIA_pcr			((volatile uint8_t *)0xFFFFFE6C)
#define	sheila_USRVIA_ifr			((volatile uint8_t *)0xFFFFFE6D)
#define	sheila_USRVIA_ier			((volatile uint8_t *)0xFFFFFE6E)
#define	sheila_USRVIA_ora_nh		((volatile uint8_t *)0xFFFFFE6F)


#define VIA_IxR_FLAG				0x80
#define VIA_IxR_T1					0x40
#define VIA_IxR_T2					0x20
#define VIA_IxR_CB1					0x10
#define VIA_IxR_CB2					0x08
#define VIA_IxR_SR					0x04
#define VIA_IxR_CA1					0x02
#define VIA_IxR_CA2					0x01


#define LATCH_0_SOUND 		0
#define LATCH_1_SPEECH_RS	1
#define LATCH_2_SPEECH_WS	2
#define LATCH_3_KEYSCAN		3
#define LATCH_4_C0		4
#define LATCH_5_C1		5
#define LATCH_6_LED_CAPS	6
#define LATCH_7_LED_SHIFT	7
#define LATCH_8_ON		8

#endif