.data 
	getIntegers :.asciiz "Enter 3 integer and also,after writing every integer press enter:\n"
	message:.asciiz "Your integers are: "
	resultMessage :.asciiz "\nThe numbers between the given first and second integers that can be exactly divided by the third given number are:\n"
	gap: .asciiz " "
	arr:.word 12 24 3
.text
	.globl main
main5:

#prints message that is in given address on screen.
li $v0,4
la $a0,getIntegers
syscall

#gets first integer from user.
li $v0,5
syscall
move $a1,$v0

#gets second integer from user.
li $v0,5
syscall
move $a2,$v0

#gets third integer from user.
li $v0,5
syscall
move $a3,$v0

#prints message that is in given address on screen.
li $v0,4
la $a0,message
syscall

#prints first integer on screen to show it.
li $v0,1
move $a0,$a1
syscall

#prints message that is in given address on screen.
li $v0,4
la $a0,gap
syscall

#prints  second  integer on screen to show it.
li $v0,1
move $a0,$a2
syscall

#prints message that is in given address on screen.
li $v0,4
la $a0,gap
syscall

#prints  third integer on screen to show it.
li $v0,1
move $a0,$a3
syscall

#prints message that is in given address on screen.
li $v0,4
la $a0,resultMessage
syscall

#arrange arguments of showDivisibleNumber function according to which number is bigger. 
slt	$t0,$a1,$a2
beq $t0,$zero,switch
jal ShowDivisibleNumbers  #calls function to show divisible numbers  between first and second number.
jal exit  #calls function to terminate program.

switch: #function arguments are arranged that case first entered integer is bigger than second.
	move $t1,$a2
	move $a2,$a1
	move $a1,$t1
	jal ShowDivisibleNumbers
	jal exit  #calls function to terminate program.


ShowDivisibleNumbers:
	move $s1,$a1
	move $s2,$a2
	move $s3,$a3
	
	loop:		
		div $t3,$s1,$s3
		mfhi $t4 #remain from  division.
		beq $s1,$s2,lastControl #if current element is last element,jump given address to make last control.
		beq $t4,$zero,print #if third number is divided current element,jump given address to print current element.
		addi $s1,$s1,1	
		j loop	
	print:
		#print element on screen.
		li $v0, 1
		move $a0, $s1
		syscall	
		
		#print gap on screen.
		li $v0,4
		la $a0,gap
		syscall
		 
		addi $s1,$s1,1	 #advance index.
		j loop	 #repeat loop
		
	lastControl:
		bne  $t4,$zero,return  #if last element is not diveded given third element, jump given address to terminate function.
		
		#print last element that is  diveded given third element.
		li $v0, 1
		move $a0, $s1 
		syscall	
		
		jr $ra	

	return:
		jr $ra		
#function for terminating program.			
exit:
	li $v0,10
	li $a0,1
	syscall					
				
