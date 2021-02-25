.data 
    str1:  .asciiz "Enter file name with extension such as filename.asm :"
	newline:.asciiz "\n\n"
	maxCharacter: .word 256
	buffer: .space 20
.text
	.globl main
main:

loop: 
		
	#Prints message that is in given address on screen.
	la $a0, str1    
	li $v0, 4
	syscall

	#takes file name from user.
	li $v0,8
	la $a0,buffer
	lw $a1,maxCharacter
	syscall
	
	#This is CreateProcess syscall which loads an assembly file from the disk and executes it, after return back. 
	la $a0,buffer
	li $v0,18
	syscall

	#Prints new line on screen.
	la $a0, newline   
	li $v0, 4
	syscall

	j loop

	