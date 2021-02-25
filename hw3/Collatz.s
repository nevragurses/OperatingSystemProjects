.data 
	message:  .asciiz "\n~~~~~~~IN COLLATZ FILE~~~~~~~\n"
	gap: .asciiz " "
	newLine: .asciiz "\n"
	twoDots: .asciiz ": "
	end:.asciiz "\n\nEND OF COLLATZ FILE. \n"
.text
	.globl main
	
main3:

#prints message that is in given address on screen.
li $v0,4
la $a0,message
syscall

collatz:
	li $t6,0 
	li $s5,26 #last  number 25.
	li $s1,2 #for division 2. 
	li $s2,1  
	numbers:
		addi $t6,$t6,1	
		move $s0,$t6
		
		#if current number equal 26,exit loop.
		beq $s0,$s5,exitFunc 
		
		#prints newline on screen.
		li $v0,4
		la $a0,newLine
		syscall
		
		#prints current number on screen.
		li $v0, 1
		move $a0,$s0
		syscall	
		
		#prints double dot that is ":" on screen
		li $v0,4
		la $a0,twoDots
		syscall
		
		beq $s0,$s2,specialPrint
	
	loop:
		beq $s0,$s2,numbers #if current sequence number equal 1,go next number.
		
		div $s0,$s1 # n/2 operation.
		mfhi $t0 #remainder of n/2.
		
		beq $t0,$zero,even #if number is even,jump even location.
		
		#If number is odd that number becomes 3*n+1
		li $t3,3
		mul $t4,$t3,$s0 #3*n
		addi $t4,$t4,1 #3*n+1
		
		#prints 3*n+1 on screen.
		li $v0, 1
		move $a0,$t4
		syscall	
		
		#print gap on screen.
		li $v0,4
		la $a0,gap
		syscall
		
		move $s0,$t4 #update number.
		
		j loop	
			
	even:
		mflo $t2 #result of n/2.
		
		#prints n/2 on screen.
		li $v0, 1
		move $a0,$t2
		syscall	
		
		#prints gap on screen.
		li $v0,4
		la $a0,gap
		syscall
		
		move $s0,$t2 #update number.
			
		j loop

	#for number 1.	
	specialPrint:
		li $v0, 1
		move $a0,$s0
		syscall	
		
		j numbers	

	#exiting from function.
	exitFunc:
		li $v0,4
		la $a0, end  
		syscall
	 
		li $v0, 22
		syscall	

	
	