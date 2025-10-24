
//note: the order has been changed substantially WRT BBC Basic for 32 bit pointers and alignment reasons

		.struct 0
			
ZP_LOMEM:		.space 4		
ZP_VARTOP:		.space 4	
ZP_BAS_SP:		.space 4	
ZP_HIMEM:		.space 4	
ZP_TXTPTR:		.space 4	
ZP_TOP:			.space 4	
ZP_PAGE:		.space 4		// change - actually a 32 bit pointer not just the page 
ZP_ERR_VECT:		.space 4	

ZP_GEN_PTR:		.space 4
ZP_MOS_ERROR_PTR_QRY:	.space 4

ZP_TXTPTR2:		.space 4
ZP_TXTOFF2:		.space 4
ZP_READ_PTR:		.space 4

ZP_INT_WA:		.space 4
ZP_INT_WA_B:		.space 4	//DOESNT ON RISC-V: note overlaps ZP_NAMELENORVT and ZP_FP_WA_B sign, exponent
ZP_INT_WA_C:		.space 4	//DOESNT ON RISC-V: note overlaps ZP_FP_WA_B mantissa


ZP_NAMELENORVT:		.space 4	//RISC-V: allocated own space instead of sharing


ZP_ESCPTR:		.space 4	// new save pointer to escape flag
ZP_EXTRA_SAVE:		.space 4	// get rid if possible? (used in cmdFOR)


ZP_FP_TMP_PTR1:		.space 4
ZP_FP_TMP_PTR2:		.space 4

NEW_WKSP_SP:		.space 4

ZP_ERL:			.space 2

ZP_FPA:			.space 5	
ZP_FPB:			.space 6
ZP_FP_TMP:		.space 6	//?

ZP_RND_WA:		.space 5


ZP_TXTOFF:		.space 1
// order of these is assumed in cmdPRINT
ZP_PRINTBYTES:		.space 1	
ZP_PRINTFLAG:		.space 1


ZP_PRLINCOUNT:		.space 1
ZP_LISTO:		.space 1
ZP_TRACE:		.space 1
ZP_MAXTRACLINE:		.space 1
ZP_WIDTH:		.space 1
ZP_REPEAT_LVL:		.space 1
ZP_GOSUB_LVL:		.space 1
ZP_FOR_LVL_X_15:	.space 1
ZP_VARTYPE:		.space 1

//TODO: extras - in 8 bit these overlap with ZP_OPT and ZP_FPB, ZP_FPB+1, overlay some of these?
ZP_TOK_FLAGS_A:		.space 1
ZP_TOK_FLAGS_B:		.space 1
ZP_TOK_FLAGS_C:		.space 1
ZP_TOK_FLAGS_D:		.space 1

//TODO: extras
ZP_LIST_FOR_LVL:	.space 1
ZP_LIST_REP_LVL:	.space 1
ZP_LIST_F_QUO:		.space 1
ZP_LIST_F_SHO:		.space 1

//	ZP_OPT:			.space 1
//	ZP_ASS_OPBUF:		.space 1	;; overlaps ZP_DUNNO, ZP_INT_WA stores opcode/postbytes being assembled
//	ZP_DUNNO:		EQU $29 assembler
ZP_STRBUFLEN:		.space 1
//	??ZP_SAVE_BUF:		EQU $37		;; note this overlaps 
ZP_ASS_LIST_OPLEN:	.space 1
//	ZP_ASS_OPLEN:		EQU $39		;; length of opcode + postbytes


//	ZP_ASS_OPSAVED:		EQU $3A		;; start of last saved instruction (used in print)
//	ZP_ASS_OP_IX:		EQU ZP_FPB + 9

//ZP_ASS_LBLEND:		EQU $4E
//ZP_TMP:			EQU $4E		; new temporary save / loop counter area
//ZP_DUNNO_WA:		EQU $50	


BASWKSP_FPTEMP1:		.space 5
BASWKSP_FPTEMP2:		.space 5
BASWKSP_FPTEMP3:		.space 5
BASWKSP_FPTEMP4:		.space 5


	.align 8
BASWKSP_INTVAR:
			.space 0x20*4
BASWKSP_DYNVAR_HEADS:
			.space 0x40*4		// was 0x40*2 in 8 bit basic

//offset is from DYNVAR_HEADS
#define BASWKSP_DYNVAR_off_PROC	0x76	
#define BASWKSP_DYNVAR_off_FN	0x78	

BASWKSP_REPEATSTACK:	.space 0x28
BASWKSP_FORSTACK:	.space 0xA4
BASWKSP_GOSUBSTACK:	.space 0x34
BASWKSP_STRING:		.space 0x100					

#define FORSTACK_ITEM_SIZE	15
#define FORSTACK_MAX_ITEMS	10
#define FORSTACK_OFFS_VARPTR	0
#define FORSTACK_OFFS_VARTYPE	2
#define FORSTACK_OFFS_STEP	3	// 5 bytes for real, 4 for int
#define FORSTACK_OFFS_TO	8	// 5 bytes for real, 4 for int
#define FORSTACK_OFFS_LOOP	13	// pointer to start of loop instructions

#define GOSUBSTACK_MAX		17

BAS_InBuf:		.space 0x100

#define VAR_P_PERCENT (BASWKSP_INTVAR + 0x40)
#define VAR_O_PERCENT (BASWKSP_INTVAR + 0x3C)

	//TODO: in 8 bit basic this is overlaid with other stuff - look at copying that?
OSWORD_BUF:		.space 0x80



