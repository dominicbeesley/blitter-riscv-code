			//  Meaning of ZP_FP_WA_B and ZP_FP_WA_B + 1 in tokenizer loop
			//  ZP_FP_WA_B	+0	+1	State is
			//		FF	0	Middle of statement mode
			//		0	0	Start of statement mode
			//		-	FF	Expect line number next

//			  TOKEN TABLE
//			  ===========
//			  string, token (b7=1), flag
//			
//			  Token flag
//			  Bit 0 - Conditional tokenisation (don't tokenise if followed by an alphabetic character).
//			  Bit 1 - Go into "Middle of Statement" mode.
//			  Bit 2 - Go into "Start of Statement" mode.
//			  Bit 3 - FN/PROC keyword - don't tokenise the name of the subroutine.
//			  Bit 4 - Start tokenising a line number now (after a GOTO, etc...).
//			  Bit 5 - Don't tokenise rest of line (REM , DATA, etc...) ; DB: JGH had this as bit 6
//			  Bit 6 - Pseudo variable flag - add 0x40 to token if at the start of a 
//				   statement/hex number i.e. PAGE
//			  Bit 7 - Unused - externally used for quote toggle.

#define TOK_FLAG_CONDITIONAL 0x01
#define BIT_FLAG_CONDITIONAL 0

#define TOK_FLAG_NEXT_MID 0x02
#define BIT_FLAG_NEXT_MOD 1

#define TOK_FLAG_NEXT_START 0x04
#define BIT_FLAG_NEXT_START 2

#define TOK_FLAG_FNPROC 0x08
#define BIT_FLAG_FNPROC 3

#define TOK_FLAG_NEXTLINENO 0x10
#define BIT_FLAG_NEXTLINENO 4


#define TOK_FLAG_SKIP_EOL 0x20
#define BIT_FLAG_SKIP_EOL 5

#define TOK_FLAG_PSEUDO_VAR 0x40
#define BIT_FLAG_PSEUDO_VAR 6

