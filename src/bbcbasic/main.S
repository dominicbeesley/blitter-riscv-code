	#include "oslib.S"


	.globl main


	.section .text, "x"
main:	mv	s1, ra
	la	a1, str
lp:	lb	a0, 0(a1)
	beqz	a0, sk
	addi	a1, a1, 1
	li	a7, OS_WRCH
	ecall
	j	lp
sk:

	j	exit



	.section .rodata

str:	.asciz "Hello"



