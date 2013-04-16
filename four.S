	.data

	.text
	.set	noreorder
	.align	2
	.set	nomips16


	.globl	four
	.ent	four
	.type	four, @function
four:
	li	$v0, 4
	jr	$ra
	nop	

	.end	four
	.size	four, .-four

