	.data

	.abicalls
	.text
	.set	nomips16


	.globl	fibonacci
fibonacci:
	.ent	fibonacci
	.type	fibonacci, @function
	addi	$sp, $sp, -40
	sw	$ra, 36($sp)
	sw	$fp, 32($sp)
	move	$fp, $sp
	li	$t0, 2
	move	$t1, $a0
	slt	$t0, $t1, $t0
	beq	$t0, $zero, .L0
	nop	
	li	$t0, 1
	move	$v0, $t0
	b	.L1
	nop	
	b	.L0
.L0:	nop	
	li	$t1, 2
	move	$t2, $a0
	sub	$t1, $t2, $t1
	sw	$a0, 40($sp)
	sw	$a1, 44($sp)
	sw	$a2, 48($sp)
	sw	$a3, 52($sp)
	sw	$t0, 16($sp)
	sw	$t1, 20($sp)
	sw	$t2, 24($sp)
	sw	$t3, 28($sp)
	move	$a0, $t1
	.option pic0
	jal	fibonacci
	nop	
	.option pic2
	lw	$a0, 40($sp)
	lw	$a1, 44($sp)
	lw	$a2, 48($sp)
	lw	$a3, 52($sp)
	lw	$t0, 16($sp)
	lw	$t1, 20($sp)
	lw	$t2, 24($sp)
	lw	$t3, 28($sp)
	move	$t0, $v0
	li	$t2, 1
	move	$t3, $a0
	sub	$t2, $t3, $t2
	sw	$a0, 40($sp)
	sw	$a1, 44($sp)
	sw	$a2, 48($sp)
	sw	$a3, 52($sp)
	sw	$t0, 16($sp)
	sw	$t1, 20($sp)
	sw	$t2, 24($sp)
	sw	$t3, 28($sp)
	move	$a0, $t2
	.option pic0
	jal	fibonacci
	nop	
	.option pic2
	lw	$a0, 40($sp)
	lw	$a1, 44($sp)
	lw	$a2, 48($sp)
	lw	$a3, 52($sp)
	lw	$t0, 16($sp)
	lw	$t1, 20($sp)
	lw	$t2, 24($sp)
	lw	$t3, 28($sp)
	move	$t1, $v0
	add	$t0, $t1, $t0
	move	$v0, $t0
	b	.L1
	nop	
.L1:	move	$sp, $fp
	lw	$ra, 36($sp)
	lw	$fp, 32($sp)
	addi	$sp, $sp, 40
	jr	$ra
	nop	

	.end	fibonacci
	.size	fibonacci, .-fibonacci

