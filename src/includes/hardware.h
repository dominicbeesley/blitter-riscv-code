#ifndef __HARDWARE_H__
#define __HARDWARE_H__

/***********************************************************************
 * System VIA                                                          *
 ***********************************************************************/
#define	sheila_SYSVIA_orb			(*((volatile uint8_t *)0xFFFFFE40))
#define	sheila_SYSVIA_ora			(*((volatile uint8_t *)0xFFFFFE41))
#define	sheila_SYSVIA_ddrb			(*((volatile uint8_t *)0xFFFFFE42))
#define	sheila_SYSVIA_ddra			(*((volatile uint8_t *)0xFFFFFE43))
#define	sheila_SYSVIA_t1cl			(*((volatile uint8_t *)0xFFFFFE44))
#define	sheila_SYSVIA_t1ch			(*((volatile uint8_t *)0xFFFFFE45))
#define	sheila_SYSVIA_t1ll			(*((volatile uint8_t *)0xFFFFFE46))
#define	sheila_SYSVIA_t1lh			(*((volatile uint8_t *)0xFFFFFE47))
#define	sheila_SYSVIA_t2cl			(*((volatile uint8_t *)0xFFFFFE48))
#define	sheila_SYSVIA_t2ch			(*((volatile uint8_t *)0xFFFFFE49))
#define	sheila_SYSVIA_sr			(*((volatile uint8_t *)0xFFFFFE4A))
#define	sheila_SYSVIA_acr			(*((volatile uint8_t *)0xFFFFFE4B))
#define	sheila_SYSVIA_pcr			(*((volatile uint8_t *)0xFFFFFE4C))
#define	sheila_SYSVIA_ifr			(*((volatile uint8_t *)0xFFFFFE4D))
#define	sheila_SYSVIA_ier			(*((volatile uint8_t *)0xFFFFFE4E))
#define	sheila_SYSVIA_ora_nh		(*((volatile uint8_t *)0xFFFFFE4F))

/***********************************************************************
 * User VIA                                                          *
 ***********************************************************************/
#define	sheila_USRVIA_orb			(*((volatile uint8_t *)0xFFFFFE60))
#define	sheila_USRVIA_ora			(*((volatile uint8_t *)0xFFFFFE61))
#define	sheila_USRVIA_ddrb			(*((volatile uint8_t *)0xFFFFFE62))
#define	sheila_USRVIA_ddra			(*((volatile uint8_t *)0xFFFFFE63))
#define	sheila_USRVIA_t1cl			(*((volatile uint8_t *)0xFFFFFE64))
#define	sheila_USRVIA_t1ch			(*((volatile uint8_t *)0xFFFFFE65))
#define	sheila_USRVIA_t1ll			(*((volatile uint8_t *)0xFFFFFE66))
#define	sheila_USRVIA_t1lh			(*((volatile uint8_t *)0xFFFFFE67))
#define	sheila_USRVIA_t2cl			(*((volatile uint8_t *)0xFFFFFE68))
#define	sheila_USRVIA_t2ch			(*((volatile uint8_t *)0xFFFFFE69))
#define	sheila_USRVIA_sr			(*((volatile uint8_t *)0xFFFFFE6A))
#define	sheila_USRVIA_acr			(*((volatile uint8_t *)0xFFFFFE6B))
#define	sheila_USRVIA_pcr			(*((volatile uint8_t *)0xFFFFFE6C))
#define	sheila_USRVIA_ifr			(*((volatile uint8_t *)0xFFFFFE6D))
#define	sheila_USRVIA_ier			(*((volatile uint8_t *)0xFFFFFE6E))
#define	sheila_USRVIA_ora_nh		(*((volatile uint8_t *)0xFFFFFE6F))

/***********************************************************************
 * CRTC                                                                *
 ***********************************************************************/

#define	sheila_CRTC_ix				(*((volatile uint8_t *)0xFFFFFE00))
#define	sheila_CRTC_dat				(*((volatile uint8_t *)0xFFFFFE01))

/***********************************************************************
 * VIDEO ULA                                                           *
 ***********************************************************************/
#define sheila_VIDULA_CTL			(*((volatile uint8_t *)0xFFFFFE20))
#define sheila_VIDULA_PAL			(*((volatile uint8_t *)0xFFFFFE21))
#define sheila_NULA_CTLAUX			(*((volatile uint8_t *)0xFFFFFE22))
#define sheila_NULA_PALAUX			(*((volatile uint8_t *)0xFFFFFE23))



// VIA interrupt flags (IER, IFR)
#define VIA_IxR_FLAG				0x80
#define VIA_IxR_T1					0x40
#define VIA_IxR_T2					0x20
#define VIA_IxR_CB1					0x10
#define VIA_IxR_CB2					0x08
#define VIA_IxR_SR					0x04
#define VIA_IxR_CA1					0x02
#define VIA_IxR_CA2					0x01

// VIA PCR bits
#define VIA_PCR_CB2_CTL_MASK		0xE0
#define VIA_PCR_CB2_CTL_IN_NEG		0x00	// Input-negative active edge
#define VIA_PCR_CB2_CTL_INT_NEG		0x20	// Independent interrupt input-negative edge*
#define VIA_PCR_CB2_CTL_IN_POS		0x40	// Input-positive active edge
#define VIA_PCR_CB2_CTL_INT_POS		0x60	// Independent interrupt input-positive edge*
#define VIA_PCR_CB2_CTL_OUT_HAND	0x80	// Handshake output
#define VIA_PCR_CB2_CTL_OUT_PULSE	0xA0	// Pulse output
#define VIA_PCR_CB2_CTL_OUT_LOW		0xC0	// Low output
#define VIA_PCR_CB2_CTL_OUT_HIGH	0xE0	// High output

#define VIA_PCR_CB1_CTL_MASK		0x10
#define VIA_PCR_CB1_CTL_INT_POS		0x10	// interrupt pos edge
#define VIA_PCR_CB1_CTL_INT_NEG		0x00	// interrupt neg edge

#define VIA_PCR_CA2_CTL_MASK		0x0E
#define VIA_PCR_CA2_CTL_IN_NEG		0x00	// Input-negative active edge
#define VIA_PCR_CA2_CTL_INT_NEG		0x02	// Independent interrupt input-negative edge*
#define VIA_PCR_CA2_CTL_IN_POS		0x04	// Input-positive active edge
#define VIA_PCR_CA2_CTL_INT_POS		0x06	// Independent interrupt input-positive edge*
#define VIA_PCR_CA2_CTL_OUT_HAND	0x08	// Handshake output
#define VIA_PCR_CA2_CTL_OUT_PULSE	0x0A	// Pulse output
#define VIA_PCR_CA2_CTL_OUT_LOW		0x0C	// Low output
#define VIA_PCR_CA2_CTL_OUT_HIGH	0x0E	// High output

#define VIA_PCR_CA1_CTL_MASK		0x01
#define VIA_PCR_CA1_CTL_INT_POS		0x01	// interrupt pos edge
#define VIA_PCR_CA1_CTL_INT_NEG		0x00	// interrupt neg edge

// VIA ACR bits
#define VIA_ACR_T1_CTL_MASK			0xC0
#define VIA_ACR_T1_CTL_1SHOT_NOPB7	0x00		//Timed interrupt each time T1 is loaded, PB7 Disabled
#define VIA_ACR_T1_CTL_CONT_NOPB7	0x40		//Continuous interrupts, PB7 Disabled
#define VIA_ACR_T1_CTL_1SHOT_PB7	0x80		//Timed interrupt each time T1 is loaded, PB7 = one shot out
#define VIA_ACR_T1_CTL_CONT_PB7		0xC0		//Continuous interrupts, PB7 = Square wave output

#define VIA_ACR_T2_CTL_MASK			0x20
#define VIA_ACR_T2_CTL_TIMED_INT	0x00
#define VIA_ACR_T2_CTL_PULSE_PB6	0x20

#define VIA_ACR_SR_CTL_MASK			0x1C
#define VIA_ACR_SR_CTL_DISABLE		0x00		// Disabled
#define VIA_ACR_SR_CTL_IN_T2		0x04		// Shift in under control of T2
#define VIA_ACR_SR_CTL_IN_PHI2		0x08		// Shift in under control of PHI2
#define VIA_ACR_SR_CTL_IN_EXT		0x0C		// Shift in under control of external clock
#define VIA_ACR_SR_CTL_OUT_T2_FREE	0x10		// Shift out free running at T2 rate
#define VIA_ACR_SR_CTL_OUT_T2		0x14		// Shift out under control of T2
#define VIA_ACR_SR_CTL_OUT_PHI2		0x18		// Shift out under control of PHI2
#define VIA_ACR_SR_CTL_OUT_EXT		0x1C		// Shift out under control of external clock

#define VIA_ACR_PB_LATCH			0x02
#define VIA_ACR_PB_LATCH_OFF		0x00
#define VIA_ACR_PA_LATCH			0x01
#define VIA_ACR_PA_LATCH_OFF		0x00

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