.data 
	gap: .asciiz " "
	message:.asciiz "\nSearching element is: "
	secondMessage:.asciiz "Given integer list is: "
	findedMessage :.asciiz "\nElement is found  at index : "
	notFindedMessage: .asciiz "\nElement is not found in  list.  \n"
	arr:.word 10 5 1 12 9 8
	size:.word 7
	target: .word 12
.text
	.globl main
	
main2:

#prints message that is in given address on screen.
li $v0,4
la $a0,secondMessage 
syscall

#prints list elements on screen.
la $a0,arr
lw $a1,size
jal printSetOnScreen

#some arguments assigned before calling function.
la $a1,arr
lw $a2,target
lw $a3,size 
 
#prints message that is in given address on screen. 
li $v0,4
la $a0,message
syscall

#shows target element on screen.
li $v0,1
move $a0,$a2
syscall

#calls function to make linear search in list.
jal LinearSearch

#calls function to terminate program.
jal exit

LinearSearch : 
	addi $s0,$zero,0 # i = 0 operation.
	loop:
		#control for i < n 
		slt $s1,$s0,$a3
		beq $s1,$zero,endLoop
		
		sll $t3,$s0,2
		add $t3,$t3,$a1
		lw $t4,0($t3) # arr[i]
	
		beq $t4,$a2,finded #if current element arr[i] is equal target element,it is found.
		
		addi $s0,$s0,1 #advance index.
		j loop #repeat the loop

		#termination of function that searching element is not found.	
		endLoop:
			li $v0,4
			la $a0,notFindedMessage
			syscall
			jr $ra
			
		#termination of function that searching element is found.		
		finded:
			#prints finded message on screen.
			li $v0,4
			la $a0,findedMessage
			syscall
			
			#prints  index  of searching element.
			move $v1,$s0
			li $v0, 1
			move $a0,$v1
			syscall
			
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
		
			
