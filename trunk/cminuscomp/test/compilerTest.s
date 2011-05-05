.text
	.align 4
.globl  main
main:
	pushl	%EBP
	movl	%ESP, %EBP
main_bb1:
	movl	%EBP, %ESP
	popl	%EBP
	ret
