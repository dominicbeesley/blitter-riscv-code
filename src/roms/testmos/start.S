#ifdef FLAVOUR_PICORV32
		#include "custom_ops_picorv32.h"
#endif

#ifdef FLAVOUR_HAZARD3
		#include "custom_ops_hazard3.h"
#endif

		#include "hardware.S"

sheila			= 0xFFFFFE00

crtc_mode_0:
			.byte	0x7f				// 0 Horizontal Total	 =128
			.byte	0x50				// 1 Horizontal Displayed =80
			.byte	0x62				// 2 Horizontal Sync	 =&62
			.byte	0x28				// 3 HSync Width+VSync	 =&28  VSync=2, HSync Width=8
			.byte	0x26				// 4 Vertical Total	 =38
			.byte	0x00				// 5 Vertial Adjust	 =0
			.byte	0x20				// 6 Vertical Displayed	 =32
			.byte	0x22				// 7 VSync Position	 =&22
			.byte	0x01				// 8 Interlace+Cursor	 =&01  Cursor=0, Display=0, Interlace=Sync
			.byte	0x07				// 9 Scan Lines/Character =8
			.byte	0x67				// 10 Cursor Start Line	  =&67	Blink=On, Speed=1/32, Line=7
			.byte	0x08				// 11 Cursor End Line	  =8

crtc_mode_7:
			.byte	0x3f				// 0 Horizontal Total	 =64
			.byte	0x28				// 1 Horizontal Displayed =40
			.byte	0x33				// 2 Horizontal Sync	 =&33  Note: &31 is a better value
			.byte	0x24				// 3 HSync Width+VSync	 =&24  VSync=2, HSync=4
			.byte	0x1e				// 4 Vertical Total	 =30
			.byte	0x02				// 5 Vertical Adjust	 =2
			.byte	0x19				// 6 Vertical Displayed	 =25
			.byte	0x1b				// 7 VSync Position	 =&1B
			.byte	0x93				// 8 Interlace+Cursor	 =&93  Cursor=2, Display=1, Interlace=Sync+Video
			.byte	0x12				// 9 Scan Lines/Character =19
			.byte	0x72				// 10 Cursor Start Line	  =&72	Blink=On, Speed=1/32, Line=18
			.byte	0x13				// 11 Cursor End Line	  =19

#define ULA_MODE_0	0x9c				
#define ULA_MODE_1	0xd8				
#define ULA_MODE_2	0xf4				
#define ULA_MODE_3	0x9c				
#define ULA_MODE_4	0x88				
#define ULA_MODE_5	0xc4				
#define ULA_MODE_6	0x88				
#define ULA_MODE_7	0x4b				

#define DELAY1  lb	a7, sheila_USRVIA_ora_nh(zero)

.macro PAL ARG
		li	a0, \ARG
		sb	a0, sheila_VIDULA_PAL(zero)
.endm		


wait_vsync:	li	t0, VIA_IxR_CA1	
		sb	t0, sheila_SYSVIA_ifr(zero)
wvslp:		lb	t0, sheila_SYSVIA_ifr(zero)
		andi	t0, t0, VIA_IxR_CA1	
		beqz	t0, wvslp
		ret

crtc_poke:	sb	a0, sheila_CRTC_ix(zero)
		sb	a1, sheila_CRTC_dat(zero)
		ret

crtc_screen_addr:
		mv	s1,ra
		mv	a1,a0
		li	a0,13
		jal	crtc_poke
		li	a0,12
		srli	a1,a1,8
		jal	crtc_poke
		mv	ra,s1
		ret


crtc_poke_mode:	mv	s0, ra
		mv	t3, a0
		mv	t4, a1

		jal	wait_vsync

		li	a0, 0
		li	t2, 12
cp_lp1:		lb	a1, 0(t3)
		jal	crtc_poke
		addi	a0, a0, 1
		addi	t3, t3, 1
		bltu	a0, t2, cp_lp1
		
		mv	a0, t4
		jal	crtc_screen_addr
		mv	ra, s0
		ret		
		
vidula_poke:	sb	a0, sheila_VIDULA_CTL(zero)
		ret

wait_vsX:	mv	s1, ra
wvsxlp:		jal	wait_vsync
		addi	a0, a0, -1
		bnez	a0, wvsxlp
		mv	ra,s1
		ret


mode0:
		mv	s2, ra
		// select mode 0
		la	a0, crtc_mode_0
		li	a1, 0x3000 >> 3
		jal	crtc_poke_mode

		li	a0, ULA_MODE_0
		jal	vidula_poke

		PAL	0x0F
		PAL	0x1F
		PAL	0x2F
		PAL	0x3F
		PAL	0x4F
		PAL	0x5F
		PAL	0x6F
		PAL	0x7F

		PAL	0x8d
		PAL	0x9d
		PAL	0xad
		PAL	0xbd
		PAL	0xcd
		PAL	0xdd
		PAL	0xed
		PAL	0xfd

		mv	ra,s2
		ret

mode7:
		mv	s2, ra
// select mode 7
		la	a0, crtc_mode_7
		li	a1, (0x0C00) | (0x2000)
		jal	crtc_poke_mode

		li	a0, ULA_MODE_7
		jal	vidula_poke

		mv	ra, s2
		ret

poke_sn76489:	li	t0, 0xFF
		sb	t0, sheila_SYSVIA_ddra(zero)
		DELAY1
		sb	a0, sheila_SYSVIA_ora_nh(zero)
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		sb	x0, sheila_SYSVIA_orb(zero)
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		li	t0, 8
		sb	t0, sheila_SYSVIA_orb(zero)
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		DELAY1
		sb	x0, sheila_SYSVIA_ddra(zero)
		ret

play_vgm:	mv	t5, ra
		mv	t4, a0
playlp:		lb	a0, 0(t4)
		addi	t4, t4, 1
		beqz	a0, playend
		andi	a1, a0, 0x80
		beqz	a1, playwait
		andi	t3, a0, 0x7F
playlp2:	lb	a0, 0(t4)
		addi	t4, t4, 1
		jal	poke_sn76489
		addi	t3, t3, -1
		bnez	t3, playlp2
		j	playlp
playwait:	jal	wait_vsync
		addi	a0, a0, -1
		bnez	a0, playwait
		j	playlp
playend:	mv	ra, t5
		ret



reset_handle_enter:	


#ifdef FLAVOUR_PICORV32
		// disable all but timer, illegal instruction, bus interrupts, debug button
		li	t0,0xFFFFFFD8
		picorv32_maskirq_insn(t0, t0)
#endif
		// disable SYS and USR VIA interrupts - just in case
		li	t0, 0x7F
		sb	t0, sheila_SYSVIA_ier(zero)
		sb	t0, sheila_USRVIA_ier(zero)

		li	t0, 0x0F
		sb	t0, sheila_SYSVIA_ddrb(zero)

		// reset latch
		li	t1, 9
rllp:		sb	t0, sheila_SYSVIA_orb(zero)
		DELAY1
		addi	t0, t0, -1
		bge	t0, t1, rllp
		

#ifdef FLAVOUR_HAZARD3

		// set up the H3 custom irq controller
		li	t0, (7<<16)|(0)
		csrrs	x0, hazard3_csr_meiea, t0	// enable all external interrupts (debug, nmi, irq)

		// and the general priv mode interrupts stuff
		li	t0, 1<<11
		csrrs	x0, mie, t0			// enable external interrupts
		csrrsi	x0, mstatus, 1<<3		// enable global interrupts

#endif


main_loop:



		jal	mode7


		// copy the splash screen
		// - assume these are both word aligned
		la	a0, splash
		la	a1, 0xFFFF7C00
		li	a2, 0xFFFF7C00 + 40 * 25
splashlp:	lw	a3, 0(a0)
		nop
		nop
		nop
		nop
		sw	a3, 0(a1)
		addi	a0, a0, 4
		addi	a1, a1, 4
		bne	a1, a2, splashlp


		
		la	a0, vgmdata
		jal	play_vgm


		li	a0, 50
		jal	wait_vsX


		// scroll up
		li	t4, 0x0C00-40
		li	t3, 0x0800
scrlp:		// set screen addr
		li	a0, 0x2000
		or	a0, a0, t4
		jal	crtc_screen_addr
		li	a0, 0xFFFF7C00
		or	a0, a0, t4		
		addi	a1, a0, 40		
clp:		sw	zero,0(a0)
		addi	a0, a0, 4
		blt	a0, a1, clp

		li	a0, 2
		jal	wait_vsX

		addi	t4, t4, -40
		bgt	t4, t3, scrlp



		jal	mode0


		// file system memory screen with a pattern

		li	t0, 0xFE
		li	t1, 0xFFFF0000
		li	t2, 0xFFFF8000
flp:		sb	t0, 0(t1)
		srli	t3, t0, 7
		slli	t0, t0, 1
		or	t0, t0, t3
		addi	t1, t1, 1
		blt	t1, t2, flp

		// rotate the memory
		li	t4, 100			// iterations
rlp2:
		li	t1, 0xFFFF0000
		li	t2, 0xFFFF8000		
rlp:		lb	a0, 0(t1)
		srli	t0, a0, 7
		andi	t0, t0, 1
		slli	a0, a0, 1
		or	a0, a0, t0
		//addi	a0, a0, 1
		sb	a0, 0(t1)
		addi	t1, t1, 1
		blt	t1, t2, rlp

		jal	wait_vsync

		addi	t4, t4, -1
		bnez	t4, rlp2


		li	a0, 250
		jal	wait_vsX





		j	main_loop

hexword:	mv	s2, ra
		mv	s3, a0
		srli	a0, a0, 24
		jal	hexbyte
		srli	a0, s3, 16
		jal	hexbyte
		srli	a0, s3, 8
		jal	hexbyte
		mv	a0, s3
		mv	ra, s2
		// fall through
hexbyte:	mv	s0, ra
		mv	s1, a0
		srli	a0, a0, 4
		jal	hexnyb
		mv	a0, s1
		mv	ra, s0
		// fall through to printch
hexnyb:		and	a0, a0, 0xF
		li	a1, 10
		blt	a0, a1, hexnyb_s1
		add	a0, a0, 'a'-'0'-10
hexnyb_s1:	addi	a0, a0, '0'
		// fall through to printch
printch:	sb	a0, 0(a7)
		addi	a7, a7, 1
		//TODO: wrap?
		ret


printstr:	mv	s0, ra
		mv	s1, a0
pslp:		lb	a0, 0(s1)
		beqz	a0, pssk
		jal	printch
		addi	s1, s1, 1
		j	pslp
pssk:		mv	ra, s0
		ret



interrupts_handle_enter:

		// store registers
		sw	x1, 0(zero)
		sw	x2, 4(zero)
		sw	x3, 8(zero)
		sw	x4, 12(zero)
		sw	x5, 16(zero)
		sw	x6, 20(zero)
		sw	x7, 24(zero)
		sw	x8, 28(zero)
		sw	x9, 32(zero)
		sw	x10, 36(zero)
		sw	x11, 40(zero)
		sw	x12, 44(zero)
		sw	x13, 48(zero)
		sw	x14, 52(zero)
		sw	x15, 56(zero)
		sw	x16, 60(zero)
		sw	x17, 64(zero)
		sw	x18, 68(zero)
		sw	x19, 72(zero)
		sw	x20, 76(zero)
		sw	x21, 80(zero)
		sw	x22, 84(zero)
		sw	x23, 88(zero)
		sw	x24, 92(zero)
		sw	x25, 96(zero)
		sw	x26, 100(zero)
		sw	x27, 104(zero)
		sw	x28, 108(zero)
		sw	x29, 112(zero)
		sw	x30, 116(zero)
		sw	x31, 120(zero)

#ifdef FLAVOUR_PICORV
		picorv32_getq_insn(x1, q0)
		sw	x1, 124(zero)
		picorv32_getq_insn(x1, q1)
		sw	x1, 128(zero)
		picorv32_getq_insn(x1, q2)
		sw	x1, 132(zero)
		picorv32_getq_insn(x1, q3)
		sw	x1, 136(zero)
#endif
#ifdef FLAVOUR_HAZARD3
		csrr	x1, mcause
		sw	x1, 124(zero)
		csrr	x1, mepc
		sw	x1, 128(zero)
		csrr	x1, hazard3_csr_meicontext
		sw	x1, 132(zero)
		csrrs	x1, hazard3_csr_meipa, 0
		sw	x1, 136(zero)
#endif

		// we're not really expecting interrupts, display an error screen
		jal	mode7

		li	a7, 0xFFFF7C00			// screen pointer

		la	a0, str_irq
		jal	printstr

		li	a4, 1	// index
		li	a5, 0	// address
		li	a6, 3	// mod 3
		li	a3, 32
ilp:		li	a0, 'x'
		blt	a4, a3, skQ
		li	a0, 'Q'
skQ:		jal	printch
		mv	a0, a4
		blt	a4, a3, skQ2
		andi	a0, a0, 0xF
skQ2:		jal	hexbyte
		li	a0, ':'
		jal	printch
		lw	a0, 0(a5)		
		jal	hexword
		li	a0, ' '
		jal	printch
		addi	a6, a6, -1
		bnez	a6, isk
		li	a6, 3
		jal	printch
isk:		addi	a4, a4, 1
		addi	a5, a5, 4
		slti	a0, a4, 36
		bnez	a0, ilp

sop:		j sop


		.data
str_irq:	.asciz "An unexpected interrupt occurred:       "
		.align(4)
		
	.section reset, "ax", @progbits
reset_hw:	j reset_handle_enter
	.section irq, "ax", @progbits
irq_hw:		j interrupts_handle_enter


