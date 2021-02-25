.data 
	beforeSorted:.asciiz  "Before sorting: "
	afterSorted:.asciiz "\nAfter sorting:  "
	
	gap:.asciiz "  "
	arr:.word 3 9 4 1 10 15 14 2
	size:.word 8
.text
	.globl main
main4:

#prints message that is in given address on screen.
li $v0,4
la $a0,beforeSorted
syscall

#prints list elements that are not sorted on screen.
la $a0,arr
lw $a1,size
jal printSetOnScreen
	
#some arguments assigned before calling function.	
la $a1,arr
lw $a2,size		

#function call to make selection sort.
jal SelectionSort

#prints message that is in given address on screen.
li $v0,4
la $a0,afterSorted
syscall

#prints list elements that are sorted on screen.
la $a0,arr
lw $a1,size
jal printSetOnScreen

#function call to terminate program.
jal exit

SelectionSort:
	addi $s0,$zero,0 #i = 0
	la $s6,arr #keeps address of list.
	addi $a3,$a2,-1 #makes size-1 (n-1)
	loop:
		la $s7,arr
		slt $s1,$s0,$a3 #control for  i < n-1
		beq $s1,$zero,return
	
		addi $t0,$s0,0 # min = i
		addi $s2,$s0,1  #j = i + 1
		innerloop:
			sll $t3,$t0,2  # min*4
			add $t5,$t3,$s6
			lw $s4,0($t5)  # arr[min]
			
			sll $t2,$s2,2  # j*4
			add $t4,$t2,$s6
			lw $s3,0($t4) # arr[j]
			
			slt $t1,$s2,$a2  #contol for j < n
			beq $t1,$zero,innerExit 
			
			slt $s5,$s3,$s4 #control for arr[j] < arr[min]
			bne $s5,$zero,condition #if condition is true,jump given address to change min element.

			addi $s2,$s2,1 #advance loop
			j innerloop
		condition:
			addi $t0,$s2,0 # min = j
			addi $s2,$s2,1 # advance loop
			j innerloop

		innerExit:
			#below operations to swap the found minimum element with the first element . 
			lw $t6,0($a1)
			add $s7,$t5,0
			
			sw $s4,0($a1)
			sw $t6,0($s7)
			
			addi $s0,$s0,1
			addi $a1,$a1,4
			j loop
	return:
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
#This function to terminate program.	
exit:
	li $v0,10
	li $a0,1
	syscall	
				
			
				
				
	
		
		
			
	
	
	
	
	
	
	
	
