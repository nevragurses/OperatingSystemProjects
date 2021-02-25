.data 
	answer: .space 500
	newline: .asciiz "\n"
	two_dots: .asciiz ":"
	bye:.asciiz "\nGoodbye...\n"
	palindromeMessage:.asciiz ": Palindrome\n"
	notPalindromeMessage:.asciiz ": Not Palindrome\n"
	wantToContinue:.asciiz "\nDo you want to continue (y/n)? : \n"
	lastWord:.asciiz "\nPlease enter the last word:\n"
	no: .asciiz "n"
	wordEnd: .word 0
	asciiNewline: .word 10
	dictionary: .asciiz "aba","ada","did","level","refer","rotator","wow","mom","eye","radar","how","book","arthas","boot","fish","chapter","computer","look","keep","adventure","mother","clean","code","good","love","butterfly","computer","car","business","brother","grandmother","purpose","weather","trouble","payment","memory","cash","machine","music","development","directory","suggession","name","sleep","learning","fire","flower","friend","best","bad","worst","milk","mind","teacher","student","industry","increase","comparision","competition","education","university","primary","school","daughter","library","healthy","ocean","rainbow","umbrella","window","appreciate","attractive","handsome","different","family","clear","women","children","man","appearance","argument","definetely","seperate","restaurant","occasion","opinion","opponent","paticular","schedule","surprise","yesterday","tomorrow","today","another","morning","evening","afternoon","night","seven","last"	
	
.text
	.globl main
	
main4:
	la $a1, dictionary   #  keeps address of first word in dictionary.
	la $t0, dictionary   #  keeps address of first word in dictionary.

	li $s0,0 #first index.
	li $s1,100 #last index.

loop:
	beq $s0,$s1,userDecision
	
	#prints index of word.
	addi $s7,$s0,1
	li $v0,1 
	move $a0,$s7
	syscall
	
	#prints two dot after index.
	li $v0,4 
	la $a0,two_dots
	syscall
	
	jal findSize #finds size of word.
	move $s3,$v0  
	
	#finds whether word is palindrome or not.
	addi $s4,$s3,-1
	move $a0,$s4 
	jal Palindrome
		
	addi $s0,$s0,1
	addi $s3,$s3,1
	add $a1,$a1,$s3
	
	j loop

#Palindrome function.This function finds whether given string is palindrome or not. 
Palindrome:
	#keeping used registers in stack to protect them.
	addi $sp, $sp, -36
	sw   $ra,32($sp)
	sw   $s5, 28($sp) 
	sw   $t4, 24($sp) 
	sw   $t3, 20($sp) 
	sw   $t2, 16($sp) 
	sw   $t1, 12($sp) 
	sw   $t0, 8($sp) 
	sw   $a1, 4($sp)
	sw   $a0, 0($sp) 
	
	#starting from leftmost and rightmost corners of word. 
	move $t0,$a1 #address of first character(leftmost) of word.=> s 
	move $t1,$a0  #size of word.
	
	add $s5,$t0,$t1 #address of last character(rightmost) of word.=> e
	palindromeLoop:
		slt $t2,$t0,$s5 #Keep comparing characters while they are same.Controlling while( e > s) 
		beq $t2,$zero,end_loop
	
		lb $t3,0($t0) #getting current right character.
		lb $t4,0($s5) #getting current left character.

		bne $t3,$t4,not_palindrome #if right and left corners is not equal.Print this word is not palindrome.
	
		addi $t0,$t0,1 
		addi $s5,$s5,-1
		j palindromeLoop
	
	end_loop:
		#prints  word is palindrome on screen.
		li $v0,4
		la $a0,palindromeMessage
		syscall
		
		j end_Palindrome
	not_palindrome:
		#prints  word is not palindrome on screen.
		li $v0,4
		la $a0,notPalindromeMessage
		syscall
		
		j end_Palindrome		
	end_Palindrome:
		#release stack.
		lw   $a0, 0($sp) 
		lw   $a1, 4($sp) 
		lw   $t0, 8($sp) 
		lw   $t1, 12($sp) 
		lw   $t2, 16($sp) 
		lw   $t3, 20($sp) 
		lw   $t4, 24($sp) 
		lw   $s5, 28($sp) 
		lw   $ra, 32($sp) 
		addi $sp, $sp, 36
		jr $ra	

#This function finds length of word.	
findSize:
	#keeping used registers in stack to protect them.
	addi $sp, $sp, -20
	sw $ra,	16($sp)
	sw $a1,12($sp) 
	sw $t1,	8($sp)  
	sw $t0,	4($sp)  
	sw $a0,	0($sp) 
	
	move $t0,$a1
	li $t1,0
	li $t7,10
	sizeLoop:
		lb $a0, 0($t0)          #print current character
		beqz $a0, findedSize         #if \0 is found,end of word.
		beq $a0,$t7,findedSize         #if \n is found,end of word.    		

		addi $t0,$t0,1	#increase address.
		addi $t1, $t1, 1   #increase the counter
	
		li $v0, 11 #prints character.
		syscall
		
		j sizeLoop
	
    	findedSize:
    		move $v0,$t1 #return length of word.
    		
    		#release stack.
    		lw   $a0, 0($sp)
			lw   $t0, 4($sp)
			lw   $t1, 8($sp)
			lw   $a1, 12($sp)
			lw   $ra, 16($sp)
			addi $sp, $sp, 20
			
    		jr $ra
    		

userDecision:
	#prints message that is given address.
	li $v0,4
	la $a0,wantToContinue
	syscall
	
	#gets y/n answer from user.
	li $v0,12
	syscall
	move  $s6,$v0
	
	
	la $s0,no
	lb $s1,0($s0)
	beq $s6,$s1,exitFunc #if answer is n,exit.
	
	#if answer is y,get word from user.
	li $v0,4
	la $a0,lastWord
	syscall
	
	#reads word.
	li $v0,8
	la $a0,answer
	#li $a1,50
	syscall
	
	la $a1, answer   #   keeps address of given word.
	la $t0, answer   #   keeps address of given word.
	
	#prints index of word.
	addi $s7,$s7,1
	li $v0,1 
	move $a0,$s7
	syscall
	
	#print two dot after index.
	li $v0,4 
	la $a0,two_dots
	syscall
	
	jal findSize #finds size of word.
	move $s3,$v0  
	
	
	#finds whether word is palindrome or not.
	addi $s4,$s3,-1
	move $a0,$s4 
	jal Palindrome
	
	#j userDecision
		
exitFunc:
	#prints message that is given address.
	li $v0,4
	la $a0,bye
	syscall	
	#exit
	li $v0,22
	syscall
