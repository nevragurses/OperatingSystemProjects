Running Instructions:
    Go to spim folder that is in spimsimulator-code-r730.
    write "make" , and then write "sudo make install".
    After run Kernel flavors with "spim read SPIMOS_GTU_1.s" ,  "spim read SPIMOS_GTU_2.s" and  "spim read SPIMOS_GTU_3.s" 
REPORT:
    I implemented  INITIALIZE_SYSCALL,FORK_SYSCALL,EXECVE_SYSCALL, WAITPID_SYSCALL, END_OF_PROCESS_SYSCALL,RANDOM_GENERATOR_SYSCALL,PROCESS_EXIT_SYSCALL in syscall.cpp.

    INITIALIZE_SYSCALL: This system call  creates a init process  and initializes process table.  
    FORK_SYSCALL: This system call creates a new process that is copy of parent process.
    EXECVE_SYSCALL: This system call  replaces process's image core by given file. 
    WAITPID_SYSCALL: This system call provide to wait until end of given  process.  
    END_OF_PROCESS_SYSCALL: This system call is using end of a file.That deletes process from process table.
    RANDOM_GENERATOR_SYSCALL: This is a random number generator syscall.That creates random number that has interval of 1-3.
    PROCESS_EXIT_SYSCALL: This system call gives acknowledge of termination.


SPIMOS_GTU_1.s : In this kernel,I use 1 initialize ,3 fork and 3 execve system call.I load 3 files into memory and I provide that  program is terminated 
when all processes is terminated.

SPIMOS_GTU_2.s : In this kernel, I select a random variable using random generator syscall for which program is working.Then I use initialize syscall and also 
I use fork and execve syscall in loop and that loop is until 0 to 10 because selected program must be load 10 times in memory as different processes.

SPIMOS_GTU_3.s : In this kernel, I select 2 random variable using random generator syscall.Then I use initialize syscall and also I use 2 fork + execve syscall 
in loop and that loop is until 0 to 3.Because all of 2 processes must work 3 times.So there must be total 6 processes.I write all combinations in kernel.

IMPORTANT NOTE ACCORDING TO MY HOMEWORK: In my work,when interrupt comes,it pass to next process according to Round Robin scheduling.It selects one other process.
But according to in my work,If current process is Collatz.asm file I make waitpid so other processes is waiting until this current waitpid process to end.I use
waitpid system call if current process is collatz.asm files so my homework is working in this way.
