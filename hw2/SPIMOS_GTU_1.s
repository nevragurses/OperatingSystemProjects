.data
	file1:  .asciiz "LinearSearch.asm"
	file2:  .asciiz "BinarySearch.asm"
	file3:  .asciiz "Collatz.asm"
    end:.asciiz "\nEND OF SPIMOS_GTU_1.s FILE \n"
.text
	.globl main
main:

loop: 

	#init process	
	li $v0,18
	syscall
	

	#fork
	li $v0,19
	syscall

	#execve
	la $a0, file1
	li $v0,20
	syscall


	#fork
	li $v0,19
	syscall


	#execve
	la $a0, file2
	li $v0,20
	syscall

	#fork
	li $v0,19
	syscall


	#execve
	la $a0, file3
	li $v0,20
	syscall


	#message of end of this file.
	li $v0,4
	la $a0, end  
	syscall
	 
	#deleting process syscall when coming end of file. 
	li $v0, 22
	syscall	

	#acknowledge program's termination by calling PROCESS_EXIT syscall.
	li $v0,24
	syscall


	

	