.data 
	gap: .asciiz " "
	message:.asciiz "\nSearching element is: "
	secondMessage:.asciiz "Given integer list is: "
	findedMessage :.asciiz "\nElement is found at index :  "
	notFindedMessage: .asciiz "\nElement is not found in list.  \n"
	arr:.word 1 4 5 7 9 10 11
	size:.word 7
	target: .word 4
.text
	.globl main
main3:

#prints message that is in given address on screen.
li $v0,4
la $a0,secondMessage 
syscall

#prints  list elements on screen.
la $a0,arr
lw $a1,size
jal printSetOnScreen

#some arguments assigned before calling function.
la $a1,arr
lw $a2,target
addi $s0,$zero,0  #first index
lw $s1,size
addi $s1,$s1,-1  #last index

#prints message that is in given address on screen.
li $v0,4
la $a0,message
syscall

#shows target element on screen.
li $v0,1
move $a0,$a2
syscall

#calls function to make binary search in list.
jal BinarySearch

#calls function to terminate program.
jal exit

BinarySearch: 
	#records  some arguments in stack.
	addi $sp, $sp, -12
	sw $ra,	8($sp)  
	sw $s1,	4($sp)  
	sw $s0,	0($sp) 
	control:
		#controls to loop conditons.
		slt $s3,$s0,$s1
		bne $s3,$zero,loop
		beq $s0,$s1,loop
		j endLoop
		loop:
			#below operations to make int mid = l + (r - l) / 2 process. 
			sub $t0,$s1,$s0
			div $t1,$t0,2
			add $t2,$s0,$t1
	
			sll $t3,$t2,2 
			add $t3,$t3,$a1
			lw $t4,0($t3) #for finding arr[mid] 
	
			beq $t4,$a2,finded  #Check if target element is present at middle.
			slt $t5,$t4,$a2
			beq $t5,$zero,right
			bne $t5,$zero,left
			
		right:
			addi $s1,$t2,-1 # If target  is smaller than middle element, ignore right half.
			j control
		left:
			addi $s0,$t2,1 # If target is  greater middle element, ignore left half.
			j control

		#termination of function that searching element is not found.	
		endLoop:
			li $v0,4
			la $a0,notFindedMessage
			syscall

			#release elements from stack.
			lw   $s0, 0($sp)
			lw   $s1, 4($sp)
			lw   $ra, 8($sp)
			addi $sp, $sp, 12
			jr $ra

		#termination of function that searching element is found.		
		finded:
			#prints finded message on screen.
			li $v0,4
			la $a0,findedMessage
			syscall

			#prints  index  of searching element.
			li $v0, 1
			move $a0,$t2
			syscall

			#release elements from stack. 
			lw   $s0, 0($sp)
			lw   $s1, 4($sp)
			lw   $ra, 8($sp)
			addi $sp, $sp, 12
			jr $ra

#this function prints list  elements on screen.$a0 and $a1 is function parameters.
printSetOnScreen:
	#keep some arguments in stack.
	addi $sp, $sp, -16
	sw $ra,	12($sp)  
	sw $s2, 8($sp) 
	sw $s1, 4($sp) 
	sw $s0, 0($sp)
	
	la $s0,($a0) #address of list. 
	move $s1, $a1 #element number of list.
	li $s2,0 #index
	loopPrint:
		beq $s2, $s1, exitPrint # check for list end.
		# print list element
		li $v0, 1
		lw $a0, 0($s0) 
		syscall
	
		#for printing gap on screen.
		li $v0,4
		la $a0,gap
		syscall
		
		addi $s2, $s2, 1 # advance loop counter.
		addi $s0, $s0, 4 # advance array element.
		j loopPrint # repeat the loop.

	exitPrint:
		#release elements in stack.
		lw   $s0, 0($sp)
		lw   $s1, 4($sp)
		lw   $s2, 8($sp)
		lw   $ra, 12($sp)
		addi $sp, $sp, 16
		jr $ra	
#function for terminating program.						
exit:
	li $v0,10
	li $a0,1
	syscall			
		
			

		
