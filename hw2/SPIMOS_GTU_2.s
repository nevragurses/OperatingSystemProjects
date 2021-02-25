.data
	file1:  .asciiz "LinearSearch.asm"
	file2:  .asciiz "BinarySearch.asm"
	file3:  .asciiz "Collatz.asm"
    end:.asciiz "\nEND OF  SPIMOS_GTU_2.s FILE \n"
.text
	.globl main

main:

#taking random number syscall.
li $v0,23
syscall

move $s0,$v0

li $s1,1
li $s2,2
li $s3,3



beq $s0,$s1,collatzFile #if random number 1 , load Collatz.asm file 10 times in memory.
beq  $s0,$s2,linearSearchFile  #if random number 2 , load LinearSearch.asm file 10 times in memory.
beq $s0,$s3,binarySearchFile  #if random number 3 , load BinarySearch.asm file 10 times in memory.

#if random number 1,load Collatz.asm file 10 times in memory.
collatzFile:

	#init process	
	li $v0,18
	syscall

	li $t0,0 #index i 
	li $t1,10  # for working 10 times.

	loop1:

		beq $t0,$t1,exit
		
		#fork
		li $v0,19
		syscall


		#execve
		la $a0, file3
		li $v0,20
		syscall

		#waitpid  until 1 collatz process ends.
		li $v0,21
		syscall
	
		#increase index.
		addi $t0,$t0,1

	j loop1 #jump begin of loop.

#If random number 2,load LinearSearch.asm file 10 times in memory.
linearSearchFile:

	#init process	
	li $v0,18
	syscall

	li $t0,0 #index i 
	li $t1,10  # for working 10 times.

	loop2:

		beq $t0,$t1,exit
	
		
		#fork
		li $v0,19
		syscall

		#execve
		la $a0, file1
		li $v0,20
		syscall

		#increase index.
		addi $t0,$t0,1

	j loop2 #jump begin of loop


#If random number 3,load BinarySearch.asm file 10 times in memory.
binarySearchFile:

	#init process	
	li $v0,18
	syscall

	li $t0,0 #index i 
	li $t1,10  # for working 10 times.


	loop3:

		beq $t0,$t1,exit
		
		#fork
		li $v0,19
		syscall

		#execve
		la $a0, file2
		li $v0,20
		syscall

		#increase index
		addi $t0,$t0,1


	j loop3 #jump begin of loop.


 exit:
    
	#message of end of file.
	li $v0,4
	la $a0, end  
	syscall
	 
	#deleting process syscall when coming end of file. 
	li $v0, 22
	syscall	   

	#acknowledge program's termination by calling PROCESS_EXIT syscall.
	li $v0,24
	syscall


