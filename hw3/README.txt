Running Instructions:
    Go to spim folder that is in spimsimulator-code-r730.
    write "make" , and then write "sudo make install".
    After run Kernel flavors with "spim read SPIMOS_GTU_1.s" ,  "spim read SPIMOS_GTU_2.s" and  "spim read SPIMOS_GTU_3.s" 


IMPORTANT NOTE ACCORDING TO MY HOMEWORK: In my work,when interrupt comes,it pass to next process according to Round Robin scheduling.It selects one other process.
But according to in my work,If current process is Collatz.s file or Palindrome.s file I make waitpid so other processes is waiting until this current waitpid process to end.I use
waitpid system call if current process is Collatz.s file or Palindrome.s file.So my homework is working in this way.
