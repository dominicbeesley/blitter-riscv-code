#ifndef __HARDWARE_S__
#define __HARDWARE_S__

/***********************************************************************
 * System VIA                                                          *
 ***********************************************************************/
sheila_SYSVIA_orb 	= 0xFFFFFE40
sheila_SYSVIA_ora 	= 0xFFFFFE41
sheila_SYSVIA_ddrb 	= 0xFFFFFE42
sheila_SYSVIA_ddra 	= 0xFFFFFE43
sheila_SYSVIA_t1cl 	= 0xFFFFFE44
sheila_SYSVIA_t1ch 	= 0xFFFFFE45
sheila_SYSVIA_t1ll 	= 0xFFFFFE46
sheila_SYSVIA_t1lh 	= 0xFFFFFE47
sheila_SYSVIA_t2cl 	= 0xFFFFFE48
sheila_SYSVIA_t2ch 	= 0xFFFFFE49
sheila_SYSVIA_sr 	= 0xFFFFFE4A
sheila_SYSVIA_acr 	= 0xFFFFFE4B
sheila_SYSVIA_pcr 	= 0xFFFFFE4C
sheila_SYSVIA_ifr 	= 0xFFFFFE4D
sheila_SYSVIA_ier 	= 0xFFFFFE4E
sheila_SYSVIA_ora_nh 	= 0xFFFFFE4F

/***********************************************************************
 * User VIA                                                          *
 ***********************************************************************/
sheila_USRVIA_orb 	= 0xFFFFFE60
sheila_USRVIA_ora 	= 0xFFFFFE61
sheila_USRVIA_ddrb 	= 0xFFFFFE62
sheila_USRVIA_ddra 	= 0xFFFFFE63
sheila_USRVIA_t1cl 	= 0xFFFFFE64
sheila_USRVIA_t1ch 	= 0xFFFFFE65
sheila_USRVIA_t1ll 	= 0xFFFFFE66
sheila_USRVIA_t1lh 	= 0xFFFFFE67
sheila_USRVIA_t2cl 	= 0xFFFFFE68
sheila_USRVIA_t2ch 	= 0xFFFFFE69
sheila_USRVIA_sr 	= 0xFFFFFE6A
sheila_USRVIA_acr 	= 0xFFFFFE6B
sheila_USRVIA_pcr 	= 0xFFFFFE6C
sheila_USRVIA_ifr 	= 0xFFFFFE6D
sheila_USRVIA_ier 	= 0xFFFFFE6E
sheila_USRVIA_ora_nh 	= 0xFFFFFE6F

sheila_CRTC_ix		= 0xFFFFFE00
sheila_CRTC_dat		= 0xFFFFFE01

sheila_VIDULA_CTL	= 0xFFFFFE20
sheila_VIDULA_PAL	= 0xFFFFFE21
sheila_NULA_CTLAUX	= 0xFFFFFE22
sheila_NULA_PALAUX	= 0xFFFFFE23



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