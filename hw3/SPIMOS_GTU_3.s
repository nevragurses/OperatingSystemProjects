.data
    #Global variables that are actually integers.
    processCount :.word 4 #total process count.
    currentRunning:.word 4 #current running process id.
    waitpidReturn :.word 4 #for waitpid return value.
    waitpidControl :.word 4  #for waitpid control.
	flag : .word 4 #for context switch.
    #Process table that includes 1 init process and 9 child  process.Each of them 264 byte.
    ProcessTable:.align 2
                 .space 2640
    newline: .asciiz "\n"
    file1:  .asciiz "LinearSearch.s"
	file2:  .asciiz "BinarySearch.s"
	file3:  .asciiz "Collatz.s"
    file4:  .asciiz "Palindrome.s"
    end:.asciiz "\nEND OF  SPIMOS_GTU_3.s FILE \n"
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
#taking second random number syscall.
li $v0,23
syscall
move $s5,$v0

#taking third  random number syscall.
li $v0,23
syscall
move $s6,$v0


li $s1,1
li $s2,2
li $s3,3
li $s4,4

#go true locations according to random numbers.
beq $s0,$s1,second
beq $s0,$s2,third
beq $s0,$s3,fourth
beq $s0,$s4,first

second:
    li $s5,2
    li $s6,3
    j newLocation
third:
    li $s5,3
    li $s6,4
    j newLocation
fourth:
    li $s5,4
    li $s6,1
    j newLocation   
           
first:
    li $s5,1
    li $s6,2
    j newLocation  


newLocation:

beq $s0,$s1,first_Selection_collatzFile  #if first random number 1,first file is Collatz.s file.
beq  $s0,$s2,first_Selection_linearSearchFile  #if first random number 2,first file is LinearSearch.s file.
beq $s0,$s3,first_Selection_binarySearchFile  #if first random number 3,first file is BinarySearch.s file.
beq $s0,$s4,first_Selection_palindromeFile  #if first random number 4,first file is Palindrome.s file.


first_Selection_collatzFile: #If first random number 1,first  is Collatz.s file.

	#init process	
	li $v0,18
	syscall

	move $a0,$v0

	li $t0,0
	li $t1,3  #for working 3 times per process.

	loop1:

        beq $t0,$t1,exit
        
        #fork for first file.
        li $v0,19
        syscall

        #execve for first file.
        la $a0, file3
        li $v0,20
        syscall

        jal continue #function that makes fork and execve  for second and third process.

        addi $t0,$t0,1
        j loop1

first_Selection_linearSearchFile: #If first random number 2,first  is LinearSearch.s file.

	#init process
	li $v0,18
	syscall

	move $a0,$v0

	li $t0,0
	li $t1,3  #for working 3 times per process.

	loop2:

        beq $t0,$t1,exit
        
        #fork for first file.
        li $v0,19
        syscall

        #execve for first file.
        la $a0, file1
        li $v0,20
        syscall
        
        jal continue #function that makes fork and execve  for second and third process.
        
        addi $t0,$t0,1
        j loop2
first_Selection_binarySearchFile: #If first random number 3,first  is BinarySearch.s file.

	#init process.
	li $v0,18
	syscall

	move $a0,$v0

	li $t0,0
	li $t1,3 #for working 3 times per process.

	loop3:

        beq $t0,$t1,exit
        
        #fork for first file.
        li $v0,19
        syscall

        #execve for first file.
        la $a0, file2
        li $v0,20
        syscall

        jal continue #function that makes fork and execve  for second and third process.

        addi $t0,$t0,1
        j loop3

first_Selection_palindromeFile:  #If first random number 4,first  is Palindrome.s file.

	#init process.
	li $v0,18
	syscall

	move $a0,$v0

	li $t0,0
	li $t1,3 #for working 3 times per process.

	loop4:

        beq $t0,$t1,exit
        
        #fork for first file.
        li $v0,19
        syscall

        #execve for first file.
        la $a0, file4
        li $v0,20
        syscall

        jal continue #function that makes fork and execve  for second and third process.

        addi $t0,$t0,1
        j loop4
#For second and third files:
continue: 
        #open stack
        addi $sp, $sp, -4
	    sw $ra,	0($sp)  

        beq $s5,$s1,collatzSelection  #if second random number 1, second file is Collatz.s file.
        beq $s5,$s2,linearSelection   #if second random number 2, second file is LinearSearch.s file.
        beq $s5,$s3,binarySelection   #if second random number 3, second file is BinarySearch.s file.
        beq $s5,$s4,palindromeSelection   #if second random number 4, second file is Palindrome.s file.
        collatzSelection:
            jal collatzSelect #fork and execve for Collatz file that is second random process.
            j secondDecision
        linearSelection:
            jal linearSelect #fork and execve for LinearSearch file that is second random process.
            j secondDecision
        binarySelection:
            jal binarySelect #fork and execve for BinarySearch file that is second random process.
            j secondDecision
        palindromeSelection: #fork and execve for Palindrome file that is second random process.
            jal palindromeSelect   
            j secondDecision 

        secondDecision:   
            beq $s6,$s1,thirdFileCollatz  #if third random number 1, third file is Collatz.s file.
            beq $s6,$s2,thirdFileLinear   #if third random number 2, third file is LinearSearch.s file.
            beq $s6,$s3,thirdFileBinary  #if third random number 3, third file is BinarySearch.s file.
            beq $s6,$s4,thirdFilePalindrome   #if third random number 4, third file is Palindrome.s file.

        thirdFileCollatz:
            jal collatzSelect #fork and execve for Collatz file that is third random process.

            #release stack
            lw   $ra, 0($sp)
            addi $sp, $sp, 4

            jr $ra
        thirdFileLinear:
           
            jal linearSelect #fork and execve for LinearSearch file that is third random process.
            #release stack.
            lw   $ra, 0($sp)
            addi $sp, $sp, 4

            jr $ra   
        thirdFileBinary:
            jal binarySelect #fork and execve for BinarySearch file that is third random process.

            #release stack
            lw   $ra, 0($sp)
            addi $sp, $sp, 4

            jr $ra
        thirdFilePalindrome:
            jal palindromeSelect  #fork and execve for Palindrome file that is third random process.

            #release stack
            lw   $ra, 0($sp)
            addi $sp, $sp, 4

            jr $ra  
#If selected file is Collatz file; fork ,execve and waitpid for Collatz process.
collatzSelect: 
    #open stack.
    addi $sp, $sp, -4
	sw $ra,	0($sp)

    #fork for collatz file.
    li $v0,19
    syscall

    #execve for collatz file.
    la $a0, file3
    li $v0,20
    syscall

    #waitpid until 1 collatz process end.
    li $v0,21
    syscall

    #release stack.
    lw   $ra, 0($sp)
    addi $sp, $sp, 4

    jr $ra  #jump called place.
#If selected file is BinarySearch file; fork ,execve for BinarySearch process.
binarySelect:
    #Open stack
    addi $sp, $sp, -4
	sw $ra,	0($sp)   

    #fork for BinarySearch  file.
    li $v0,19
    syscall

    #execve for BinarySearch  file.
    la $a0, file2
    li $v0,20
    syscall

    #release stack.
    lw   $ra, 0($sp)
    addi $sp, $sp, 4
    jr $ra #return called address.
#If selected file is LinearSearch file; fork ,execve for LinearSearch process.
linearSelect:
    #open stack.
    addi $sp, $sp, -4
	sw $ra,	0($sp)

    #fork for LinearSearch file.
    li $v0,19
    syscall

    #execve for LinearSearch file.
    la $a0, file1
    li $v0,20
    syscall

    #release stack.
    lw   $ra, 0($sp)
    addi $sp, $sp, 4
    jr $ra #return called address.
#If selected file is Palindrome file; fork ,execve and waitpid for Palindrome process.
palindromeSelect:
    #open stack.
    addi $sp, $sp, -4
	sw $ra,	0($sp)

    #fork for Palindrome file.
    li $v0,19
    syscall

    #execve for Palindrome file.
    la $a0, file4
    li $v0,20
    syscall

    #waitpid until 1 palindrome process end.
    li $v0,21
    syscall

    #release stack.
    lw   $ra, 0($sp)
    addi $sp, $sp, 4
    jr $ra #return called address.



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
