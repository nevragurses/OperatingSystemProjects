.data
	file1:  .asciiz "LinearSearch.asm"
	file2:  .asciiz "BinarySearch.asm"
	file3:  .asciiz "Collatz.asm"
    end:.asciiz "\nEND OF SPIMOS_GTU_3.s FILE \n"
.text
	.globl main


main:

#taking random number syscall.
li $v0,23
syscall
move $s0,$v0

#taking random number syscall.
li $v0,23
syscall
move $s5,$v0

li $s1,1
li $s2,2
li $s3,3


beq $s0,$s1,second
beq $s0,$s2,third
beq $s0,$s3,first

second:
    li $s5,2
    j newLocation
third:
    li $s5,3
    j newLocation
first:
    li $s5,1
    j newLocation        


newLocation:

beq $s0,$s1,first_Selection_collatzFile  #if first random number 1,first file is Collatz.asm file.
beq  $s0,$s2,first_Selection_linearSearchFile  #if first random number 2,first file is LinearSearch.asm file.
beq $s0,$s3,first_Selection_binarySearchFile  #if first random number 3,first file is BinarySearch.asm file.


first_Selection_collatzFile:

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
        
        beq $s5,$s1,collatzSelection1  #if second random number 1, second file is Collatz.asm file.
        beq $s5,$s2,linearSelection1   #if second random number 2, second file is LinearSearch.asm file.
        beq $s5,$s3,binarySelection1   #if second random number 3, second file is BinarySearch.asm file.

        collatzSelection1:

            #fork for second file.
            li $v0,19
            syscall

            #execve for second file.
            la $a0, file3
            li $v0,20
            syscall

            #waitpid until 1 collatz process end.
            li $v0,21
            syscall

            addi $t0,$t0,1 #increase index.

            j loop1 #jump begin of loop.

        binarySelection1:
             
            #fork for second file.
            li $v0,19
            syscall

            #execve for second file.
            la $a0, file2
            li $v0,20
            syscall

            addi $t0,$t0,1 #increase index.
            j loop1 #jump begin of loop.

        linearSelection1:

            #fork for second file.
            li $v0,19
            syscall

            #execve for second file.
            la $a0, file1
            li $v0,20
            syscall

            addi $t0,$t0,1 #increase index.
            j loop1  #jump begin of loop.


first_Selection_linearSearchFile:

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
        

        beq $s5,$s1,collatzSelection2 #if second random number 1, second file is Collatz.asm file.
        beq $s5,$s2,linearSelection2   #if second random number 2, second file is LinearSearch.asm file.
        beq $s5,$s3,binarySelection2    #if second random number 3, second file is BinarySearch.asm file.

        collatzSelection2:
            #fork for second file.
            li $v0,19
            syscall

            #execve for second file.
            la $a0, file3
            li $v0,20
            syscall

            #waitpid until 1 collatz process end.
            li $v0,21
            syscall

            addi $t0,$t0,1 #increase index.
            j loop2  #jump begin of loop.

        binarySelection2:
            #fork for second file.
            li $v0,19
            syscall

            #execve for second file.
            la $a0, file2
            li $v0,20
            syscall

            addi $t0,$t0,1 #increase index.
            j loop2 #jump begin of loop.

        linearSelection2:
            #fork for second file.
            li $v0,19
            syscall

            #execve for second file.
            la $a0, file1
            li $v0,20
            syscall

            addi $t0,$t0,1 #increase index.
            j loop2 #jump begin of loop.

first_Selection_binarySearchFile:

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
        
        beq $s5,$s1,collatzSelection3 #if second random number 1, second file is Collatz.asm file.
        beq $s5,$s2,linearSelection3 #if second random number 2, second file is LinearSearch.asm file.
        beq $s5,$s3,binarySelection3  #if second random number 3, second file is BinarySearch.asm file.

        collatzSelection3:

            #fork for second file.
            li $v0,19
            syscall

            #execve for second file.
            la $a0, file3
            li $v0,20
            syscall

            #waitpid for 1 collatz process end.
            li $v0,21
            syscall

            addi $t0,$t0,1 #increase index.
            j loop3 #jump begin of loop.

        binarySelection3:

            #fork for second file.
            li $v0,19
            syscall

            #execve for second file.
            la $a0, file2
            li $v0,20
            syscall

            addi $t0,$t0,1 #increase index.
            j loop3 #jump begin of loop.

        linearSelection3:

            #fork for second file.
            li $v0,19
            syscall

            #execve for second file.
            la $a0, file1
            li $v0,20
            syscall

            addi $t0,$t0,1 #increase index.
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


