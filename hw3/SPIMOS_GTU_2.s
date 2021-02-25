.data
    #Global variables that are actually integers.
    processCount :.word 4 #total process count.
    currentRunning:.word 4 #current running process id.
    waitpidReturn :.word 4 #for waitpid return value.
    waitpidControl :.word 4  #for waitpid control.
	flag : .word 4 #for context switch.
    #Process table that includes 1 init process and 1 child process in 5 times.Each of them 264 byte.
    ProcessTable:.align 2
                 .space 1584
    newline: .asciiz "\n"
    file1:  .asciiz "LinearSearch.s"
	file2:  .asciiz "BinarySearch.s"
	file3:  .asciiz "Collatz.s"
    file4:  .asciiz "Palindrome.s"
    end:.asciiz "\nEND OF  SPIMOS_GTU_2.s FILE \n"
.text 
	.globl main
main:
#taking random number syscall.
li $v0,23
syscall
j go

contextSwitch: #context switch part.This part works only called time.
		la $a0,flag 
		li $t7,1
		sw $t7,($a0) #update flag for timer handler.
 
		li $v0,24 #call context_switch syscall.
		syscall 

		jr $a3 #return address that is called address.
go:
move $s0,$v0

li $s1,1
li $s2,2
li $s3,3
li $s4,4

beq $s0,$s1,collatzFile #if random number 1 , load Collatz.s file 5 times in memory.
beq  $s0,$s2,linearSearchFile  #if random number 2 , load LinearSearch.s file 5 times in memory.
beq $s0,$s3,binarySearchFile  #if random number 3 , load BinarySearch.s file 5 times in memory.
beq $s0,$s4,palindromeFile  #if random number 4 , load Palindrome.s file 5 times in memory.


#if random number 1,load Collatz.s file 5 times in memory.
collatzFile:

	#init process	
	li $v0,18
	syscall

	li $t0,0 #index i 
	li $t1,5  # for working 5 times.

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

#If random number 2,load LinearSearch.s file 5 times in memory.
linearSearchFile:

	#init process	
	li $v0,18
	syscall

	li $t0,0 #index i 
	li $t1,5  # for working 5 times.

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


#If random number 3,load BinarySearch.s file 5 times in memory.
binarySearchFile:

	#init process	
	li $v0,18
	syscall

	li $t0,0 #index i 
	li $t1,5  # for working 5 times.


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

#If random number 4,load Palindrome.s file 5 times in memory.
palindromeFile:
    #init process	
	li $v0,18
	syscall

	li $t0,0 #index i 
	li $t1,5  # for working 5 times.

	loop4:

		beq $t0,$t1,exit
		
		#fork
		li $v0,19
		syscall


		#execve
		la $a0, file4
		li $v0,20
		syscall

		#waitpid  until 1 Palindrome process ends.
		li $v0,21
		syscall
	
		#increase index.
		addi $t0,$t0,1

	j loop4 #jump begin of loop.

 exit:
    
	#message of end of file.
	li $v0,4
	la $a0, end  
	syscall
	 
	#deleting process syscall when coming end of file. 
	li $v0, 22
	syscall	   

	#acknowledge program's termination by calling PROCESS_EXIT syscall.
	li $v0,25
	syscall