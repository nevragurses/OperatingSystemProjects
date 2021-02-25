.data
    #Global variables that are actually integers.
    processCount :.word 4 #total process count.
    currentRunning:.word 4 #current running process id.
    waitpidReturn :.word 4 #for waitpid return value.
    waitpidControl :.word 4  #for waitpid control.
	flag : .word 4 #for context switch.
	#Process table that includes 1 init process and 4 child processes.Each of them 264 byte.
    ProcessTable:.align 2
                 .space 1320
    newline: .asciiz "\n"
    file1:  .asciiz "LinearSearch.s"
	file2:  .asciiz "BinarySearch.s"
	file3:  .asciiz "Collatz.s"
    file4: .asciiz "Palindrome.s"
	end:.asciiz "\nEND OF  SPIMOS_GTU_1.s FILE \n"
.text 
	.globl main
main:
    #init process	
	li $v0,18
	syscall
	j continue

	contextSwitch: #context switch part.This part works only called time.
		la $a0,flag 
		li $t7,1
		sw $t7,($a0) #update flag for timer handler.
 
		li $v0,24 #call context switch syscall.
		syscall 

		jr $a3 #return address that is called address.

	continue:
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

		#waitpid  until 1 collatz process ends.
		li $v0,21
		syscall

		#fork
		li $v0,19
		syscall

		#execve
		la $a0, file4
		li $v0,20
		syscall

		#waitpid  until 1 palindrome process ends.
		li $v0,21
		syscall


		#message of end of this file.
		li $v0,4
		la $a0, end  
		syscall

		#deleting process syscall when coming end of file. 
		li $v0, 22
		syscall	

		#acknowledge program's termination by calling PROCESS_EXIT syscall.
		li $v0,25
		syscall
