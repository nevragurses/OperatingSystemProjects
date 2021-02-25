/* SPIM S20 MIPS simulator.
   Execute SPIM syscalls, both in simulator and bare mode.
   Execute MIPS syscalls in bare mode, when running on MIPS systems.
   Copyright (c) 1990-2010, James R. Larus.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

   Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

   Neither the name of the James R. Larus nor the names of its contributors may be
   used to endorse or promote products derived from this software without specific
   prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _WIN32
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdexcept>

#ifdef _WIN32
#include <io.h>
#endif
#include <iostream>
#include "spim.h"
#include "string-stream.h"
#include "spim-utils.h" 
#include "inst.h"
#include "reg.h"
#include "mem.h"
#include "data.h"
#include "sym-tbl.h"
#include "syscall.h"
#include <string>
#define ProcessTableAddr 268501012 //Process Table Address in kernels that are assembly files.
#define ProcessTableSize 264 //1 Process size in process table.
#define GlobVariablesStartAddr 268500992 //Global variables start address in kernels that are assembly files.
#define contextSwitchAddr 4194352 //Context switch address in kernels that are assembly files.
using namespace std; 


#ifdef _WIN32
/* Windows has an handler that is invoked when an invalid argument is passed to a system
   call. https://msdn.microsoft.com/en-us/library/a9yf33zb(v=vs.110).aspx

   All good, except that the handler tries to invoke Watson and then kill spim with an exception.

   Override the handler to just report an error.
*/

#include <stdio.h>
#include <stdlib.h>
#include <crtdbg.h>

void myInvalidParameterHandler(const wchar_t* expression,
   const wchar_t* function, 
   const wchar_t* file, 
   unsigned int line, 
   uintptr_t pReserved)
{
  if (function != NULL)
    {
      run_error ("Bad parameter to system call: %s\n", function);
    }
  else
    {
      run_error ("Bad parameter to system call\n");
    }
}

static _invalid_parameter_handler oldHandler;

void windowsParameterHandlingControl(int flag )
{
  static _invalid_parameter_handler oldHandler;
  static _invalid_parameter_handler newHandler = myInvalidParameterHandler;

  if (flag == 0)
    {
      oldHandler = _set_invalid_parameter_handler(newHandler);
      _CrtSetReportMode(_CRT_ASSERT, 0); // Disable the message box for assertions.
    }
  else
    {
      newHandler = _set_invalid_parameter_handler(oldHandler);
      _CrtSetReportMode(_CRT_ASSERT, 1);  // Enable the message box for assertions.
    }
}
#endif
/* Prints process table informations of current process  on screen. */
void printProcessInformations(int currentRunningProc){
    /*Addresses of sections of  Process Table in kernels that are assembly files. */
    int currentProcessTableAddr=ProcessTableAddr+(currentRunningProc*ProcessTableSize);
    unsigned int pc=currentProcessTableAddr;
    int pid=currentProcessTableAddr+4;
    int ppid=currentProcessTableAddr+8;
    unsigned int textSegAddr=currentProcessTableAddr+12;
    unsigned int dataSegAddr=currentProcessTableAddr+16;
    unsigned int stackSegAddr=currentProcessTableAddr+20;
    int state=currentProcessTableAddr+32;
    int processName=currentProcessTableAddr+164;
    /*Prints informations on screen.*/
    cout<< "\n\n-------INFORMATIONS OF PROCESS WHEN TIMER INTERRUPT COME----------\n\n";
    cout<<"Process Name:"<<(char*)mem_reference(processName)<<endl;
    cout<<"Program Counter:"<<read_mem_word(pc)<<endl;
    cout<< "Process Id:"<<read_mem_word(pid)<<endl;
    cout <<"Parent Process Id:"<<read_mem_word(ppid)<<endl;
    cout<<"Data Pointer Address:"<<read_mem_word(dataSegAddr)<<endl;
    cout<<"Stack Pointer Address:"<<read_mem_word(stackSegAddr)<<endl;
    cout<<"End of Text Pointer Address:"<<read_mem_word(textSegAddr)<<endl;
    if(read_mem_word(state)==1)
      cout<<"Process State:"<<" RUNNING "<<endl<< endl;
    else {
      cout<<"Process State:"<<" READY "<<endl<< endl;
    }
      
}
/*Saves current informations of computer(registers,pc etc)  in Process Table in kernels that are assembly files. */
void save_beforeSwitch(){
    /*Addresses of sections of Process Table in kernels that are assembly files. */
    int currentRunningNum = read_mem_word(GlobVariablesStartAddr+4);
    int currentProcessTableAddr=ProcessTableAddr+(currentRunningNum*ProcessTableSize);
    unsigned int pc=currentProcessTableAddr;
    int hi=currentProcessTableAddr+24;
    int lo=currentProcessTableAddr+28;
    int state=currentProcessTableAddr+32;
    int registersStart=currentProcessTableAddr+36;
    int processName=currentProcessTableAddr+164;
    int controlDataSeg= read_mem_word(currentProcessTableAddr+16);
    int waitpidControl=read_mem_word(GlobVariablesStartAddr+12);
    
    if( controlDataSeg!=0){ /*If process did not deleted */
      int i=0;
      int increaseAmount=0;
      /*Saves registers in process table. */
      for(i=0;i<32;++i){ 
          set_mem_word((registersStart+increaseAmount),R[i]);
          increaseAmount=increaseAmount+4;
      }    
      /*Saves current situation of computer in process table. */
      set_mem_word(pc,PC); 
      set_mem_word(hi,HI);
      set_mem_word(lo,LO);
      string s =  (char*)mem_reference(processName);
      if((s.compare("Collatz.s")==0 && waitpidControl==0) || (s.compare("Palindrome.s")==0 && waitpidControl==0)){
        set_mem_word(state,1);
        printProcessInformations(currentRunningNum);
      }
    } 
}
void SPIM_timerHandler()
{ 
    save_beforeSwitch(); //Saves current situation of computer.
    /*For prevent timer interrupt in assembly file while context switch is making by syscall in kernels that are assembly file.*/
    if(read_mem_word(GlobVariablesStartAddr+16)==1){ 
        R[REG_A3]=PC; //Save pc in register.
    }    
}  

/* Decides which syscall to execute or simulate.  Returns zero upon
exit syscall and non-zero to continue execution. */
int
do_syscall ()
{
#ifdef _WIN32
    windowsParameterHandlingControl(0);
#endif

  /* Syscalls for the source-language version of SPIM.  These are easier to
     use than the real syscall and are portable to non-MIPS operating
     systems. */

  switch (R[REG_V0])
    {
    case PRINT_INT_SYSCALL:
    {
      write_output (console_out, "%d", R[REG_A0]);      
      break;
    }
    case PRINT_FLOAT_SYSCALL:
      {
	float val = FPR_S (REG_FA0);

	write_output (console_out, "%.8f", val);
	break;
      }

    case PRINT_DOUBLE_SYSCALL:
      write_output (console_out, "%.18g", FPR[REG_FA0 / 2]);
      break;

    case PRINT_STRING_SYSCALL:
    {
      write_output (console_out, "%s", mem_reference (R[REG_A0]));
      break;
    }

    case READ_INT_SYSCALL:
      {
        static char str [256];

        read_input (str, 256);
        R[REG_RES] = atol (str);
        break;
      }

    case READ_FLOAT_SYSCALL:
      {
	static char str [256];

	read_input (str, 256);
	FPR_S (REG_FRES) = (float) atof (str);
	break;
      }

    case READ_DOUBLE_SYSCALL:
      {
	static char str [256];

	read_input (str, 256);
	FPR [REG_FRES] = atof (str);
	break;
      }

    case READ_STRING_SYSCALL:
      {
	read_input ( (char *) mem_reference (R[REG_A0]), R[REG_A1]);
	data_modified = true;
	break;
      }

    case SBRK_SYSCALL:
      {
	mem_addr x = data_top;
	expand_data (R[REG_A0]);
	R[REG_RES] = x;
	data_modified = true;
	break;
      }

    case PRINT_CHARACTER_SYSCALL:
      write_output (console_out, "%c", R[REG_A0]);
      break;

    case READ_CHARACTER_SYSCALL:
      {
        static char str [2];

        //read_input (str, 2);
        //if (*str == '\0') *str = '\n';      /* makes xspim = spim */
        str[0]=getchar();
        R[REG_RES] = (long) str[0];
        break;
      }

    case EXIT_SYSCALL:
      spim_return_value = 0;
      return (0);

    case EXIT2_SYSCALL:
      spim_return_value = R[REG_A0];	/* value passed to spim's exit() call */
      return (0);

    case OPEN_SYSCALL:
      {
#ifdef _WIN32
        R[REG_RES] = _open((char*)mem_reference (R[REG_A0]), R[REG_A1], R[REG_A2]);
#else
	R[REG_RES] = open((char*)mem_reference (R[REG_A0]), R[REG_A1], R[REG_A2]);
#endif
	break;
      }

    case READ_SYSCALL:
      {
	/* Test if address is valid */
	(void)mem_reference (R[REG_A1] + R[REG_A2] - 1);
#ifdef _WIN32
	R[REG_RES] = _read(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#else
	R[REG_RES] = read(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#endif
	data_modified = true;
	break;
      }

    case WRITE_SYSCALL:
      {
	/* Test if address is valid */
	(void)mem_reference (R[REG_A1] + R[REG_A2] - 1);
#ifdef _WIN32
	R[REG_RES] = _write(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#else
	R[REG_RES] = write(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#endif
	break;
      }
    /*This system call  creates a init process  and initializes process table in kernels that are assembly files. */  
    case INITIALIZE_SYSCALL:
    {
      int i=0;
      /*Addresses of sections of Process Table in kernels that are assembly files. */
      unsigned int pc=ProcessTableAddr;
      int pid=ProcessTableAddr+4;
      int ppid=ProcessTableAddr+8;
      unsigned int textSegAddr=ProcessTableAddr+12;
      unsigned int dataSegAddr=ProcessTableAddr+16;
      unsigned int stackSegAddr=ProcessTableAddr+20;
      int hi=ProcessTableAddr+24;
      int lo=ProcessTableAddr+28;
      int state=ProcessTableAddr+32;
      int registersStart=ProcessTableAddr+36;
      int processName=ProcessTableAddr+164;
      int processCount=GlobVariablesStartAddr;
      int currentRunning=GlobVariablesStartAddr+4;
      int waitpidControl=GlobVariablesStartAddr+12;

      mem_addr addressText,addressData; 
      addressText=starting_address();
      /*This loop to find end of text segment address. */
      while(text_seg[i]!=0){
          addressText=addressText+4;
          i=i+1;
      }
      addressData=current_data_pc();  /*Data segment address. */

      int increaseAmount=0;
      /*Below loop for record computer registers in process table in kernels that are assembly files for init process. */
      for(i=0;i<32;++i){
          set_mem_word((registersStart+increaseAmount),R[i]);
          increaseAmount=increaseAmount+4;
      }    
      /*Assigning other informations to process table in assembly files of init process. */
      set_mem_word(pc,PC);
      set_mem_word(pid,0);
      set_mem_word(ppid,-1);
      set_mem_word(hi,HI);
      set_mem_word(lo,LO);
      char name[5]="init";
      int increaseByte=0;
      /*Assign name of process as init in process table. */
      for(i=0;i<(int)strlen(name);++i){
          set_mem_byte((processName+increaseByte),name[i]);
          increaseByte=increaseByte+1;
      } 
      set_mem_word(state,1);
      set_mem_word(textSegAddr,addressText);
      set_mem_word(dataSegAddr,addressData);
      set_mem_word(stackSegAddr,R[REG_SP]);


      /*Update global variables in kernels that are assembly files.*/
      set_mem_word(processCount,1); //increase process count.
      set_mem_word(currentRunning,0); //current running is init process.
      set_mem_word(waitpidControl,0);
      set_mem_word(GlobVariablesStartAddr+16,0);
      R[REG_RES]=0;
    
      break;
    }
    /*This is fork system call that creates a new process that is copy of parent process. */
    case FORK_SYSCALL:
    {
      int i=0;
      /*Addresses of sections of Process Table in kernels that are assembly files. */
      int currentProcessNum = read_mem_word(GlobVariablesStartAddr);
      int currentProcessTableAddr=ProcessTableAddr+(currentProcessNum*ProcessTableSize);
      unsigned int pc=currentProcessTableAddr;
      int pid=currentProcessTableAddr+4;
      int ppid=currentProcessTableAddr+8;
      unsigned int textSegAddr=currentProcessTableAddr+12;
      unsigned int dataSegAddr=currentProcessTableAddr+16;
      unsigned int stackSegAddr=currentProcessTableAddr+20;
      int hi=currentProcessTableAddr+24;
      int lo=currentProcessTableAddr+28;
      int state=currentProcessTableAddr+32;
      int registersStart=currentProcessTableAddr+36;
      int processName=currentProcessTableAddr+164;
      int processCount=GlobVariablesStartAddr;
      mem_addr addressText,addressData; 
      addressText=starting_address();
      /*This loop to find end of text segment address. */
      while(text_seg[i]!=0){
          addressText=addressText+4;
          i=i+1;
      }
      addressData=current_data_pc(); /*Data segment address. */ 
      int increaseAmount=0;
      /*Below loop for record computer registers in process table in kernels that are assembly files for fork process. */
      for(i=0;i<32;++i){
          set_mem_word((registersStart+increaseAmount),R[i]);
          increaseAmount=increaseAmount+4;
      }  
      /*Assigning other informations to process table in kernels that are assembly files of fork process. */  
      set_mem_word(pc,PC);
      set_mem_word(pid,currentProcessNum);
      set_mem_word(ppid,currentProcessNum-1);
      set_mem_word(hi,HI);
      set_mem_word(lo,LO);
      char name[]="childProcess";
      int increaseByte=0;
      /*Assign name of process as childProcess in process table. */
      for(i=0;i<(int)strlen(name);++i){
          set_mem_byte((processName+increaseByte),name[i]);
          increaseByte=increaseByte+1;
      } 
      set_mem_word(state,0);
      set_mem_word(textSegAddr,addressText);
      set_mem_word(dataSegAddr,addressData);
      set_mem_word(stackSegAddr,R[REG_SP]);
     
      set_mem_word(processCount,currentProcessNum+1); //increase process count.
      break;
    }    
    /*This is execve system call that replaces process's image core by given file. */
    case EXECVE_SYSCALL:
    {
      /*Addresses of sections of Process Table in kernels that are assembly files. */
      int currentProcessNum = read_mem_word(GlobVariablesStartAddr);
      int i=0;
      int execveProcess=currentProcessNum-1;
      int currentProcessTableAddr=ProcessTableAddr+(execveProcess*ProcessTableSize);
      unsigned int pc=currentProcessTableAddr;
      int pid=currentProcessTableAddr+4;
      int ppid=currentProcessTableAddr+8;
      unsigned int textSegAddr=currentProcessTableAddr+12;
      unsigned int dataSegAddr=currentProcessTableAddr+16;
      unsigned int stackSegAddr=currentProcessTableAddr+20;
      int hi=currentProcessTableAddr+24;
      int lo=currentProcessTableAddr+28;
      int state=currentProcessTableAddr+32;
      int registersStart=currentProcessTableAddr+36;
      int processName=currentProcessTableAddr+164;
      mem_addr addressText,addressData;
     
      addressText=starting_address();
      /*This loop to find end of text segment address. */
      while(text_seg[i]!=0){
        addressText=addressText+4;
        i=i+1;
      }
      string file((char*)mem_reference (R[REG_A0])); /*file name */
      read_assembly_file(const_cast<char*>(file.c_str())); /*reads given file. */

      int increaseAmount=0;
      /*Below loop for record computer registers in process table in kernels that are assembly files for execve process. */
      for(i=0;i<32;++i){
          set_mem_word((registersStart+increaseAmount),R[i]);
          increaseAmount=increaseAmount+4;
      }    
      /*Assigning other informations in process table to change process's image core according to given file.*/
      addressData=current_data_pc();
      set_mem_word(pc,addressText);
      set_mem_word(pid,currentProcessNum-1);
      set_mem_word(ppid,currentProcessNum-2);
      R[REG_RES]=currentProcessNum-1;
      set_mem_word(hi,HI);
      set_mem_word(lo,LO);
      set_mem_word(state,0);
      set_mem_word(textSegAddr,current_text_pc());
      set_mem_word(dataSegAddr,addressData);
      set_mem_word(stackSegAddr,R[REG_SP]);
      int increaseByte=0;
      /*Assing name of process in process table according to file name. */
      for(i=0;i<(int)strlen( (const_cast<char*> (file.c_str())));++i){
          set_mem_byte((processName+increaseByte),(const_cast<char*> (file.c_str()))[i]);
          increaseByte=increaseByte+1;
      } 
      set_mem_byte((processName+increaseByte),'\0');
      break;
    }
    /*This is waitpid system call that provide to wait until end of given  process. */
    case WAITPID_SYSCALL :
    {
      /*Address of waitpid sections in kernels that are assembly files. */
      int waitpidReturnAddr=GlobVariablesStartAddr+8;
      int waitpidControAddr=GlobVariablesStartAddr+12;
     
      set_mem_word(waitpidControAddr,0); //Waitpid control.
      set_mem_word(waitpidReturnAddr,R[REG_A0]); 
      break;
    } 
    /*This is a random number generator syscall.That creates random number that has interval of 1-4 */
    case RANDOM_GENERATOR_SYSCALL:
    {
      srand (time(NULL));
      int random= rand() % 4 + 1;
      R[REG_RES]=random;

      break;
    }
    /*Context switch syscall. */
    case CONTEXT_SWITCH_SYSCALL:
    {
      /*Addresses of sections of process table in kernels that are assembly files. */
      int currentRunningNum = read_mem_word(GlobVariablesStartAddr+4);
      int currentProcessTableAddr=ProcessTableAddr+(currentRunningNum*ProcessTableSize);
      unsigned int pc=currentProcessTableAddr;
      int hi=currentProcessTableAddr+24;
      int lo=currentProcessTableAddr+28;
      int state=currentProcessTableAddr+32;
      int registersStart=currentProcessTableAddr+36;
      int processName=currentProcessTableAddr+164;
      int controlDataSeg= read_mem_word(currentProcessTableAddr+16);
      int waitpidControl=read_mem_word(GlobVariablesStartAddr+12);
      
      if( controlDataSeg!=0){ /*If process did not deleted. */
        int i=0;
        int increaseAmount=0;
        for(i=0;i<32;++i){ /*Update registers in process table before context switch.*/
            set_mem_word((registersStart+increaseAmount),R[i]);
            increaseAmount=increaseAmount+4;
        } 
        /*Update informations in process table. */   
        set_mem_word(pc,R[REG_A3]);
        set_mem_word(hi,HI);
        set_mem_word(lo,LO);
  
        string s =  (char*)mem_reference(processName);
        /*Print informations of new process on screen.*/
        if((s.compare("Collatz.s")==0 && waitpidControl==0) || (s.compare("Palindrome.s")==0 && waitpidControl==0)){
          set_mem_word(state,1);
          printProcessInformations(currentRunningNum);
        }
    }  
  }  
  /*This system call is using end of a file.That deletes process from process table.*/
  case END_OF_PROCESS_SYSCALL:
    { 
      int currentProcessNum = read_mem_word(GlobVariablesStartAddr); //current process number.
      int currentRunningNum = read_mem_word(GlobVariablesStartAddr+4); //current running process.
      /*Addresses of sections of process table.*/
      int currentProcessTableAddr=ProcessTableAddr+(currentRunningNum*ProcessTableSize);
      int state=currentProcessTableAddr+32;
      int controlDataSeg= read_mem_word(currentProcessTableAddr+16);
      int controlDataSegAddr=currentProcessTableAddr+16;
      /*If process did not deleted before;*/
      if(controlDataSeg!=0){
        set_mem_word(controlDataSegAddr,0); //Delete process by change data segment address as 0.
        currentProcessNum=currentProcessNum-1; //decrease process number.
        currentRunningNum=currentRunningNum+1; 
        /*Update new addresses after deleted process.*/
        currentProcessTableAddr=ProcessTableAddr+(currentRunningNum*ProcessTableSize);
        state=currentProcessTableAddr+32;
        set_mem_word(GlobVariablesStartAddr,currentProcessNum);
        set_mem_word(GlobVariablesStartAddr+4,currentRunningNum);
        /*If current process number is 0,all processes end,so exit.*/
        if(currentProcessNum==0){
          cout << "\nALL PROCESSES FINISHED SUCCESFULLY.\n" << endl;
          force_break=true;
          spim_return_value = R[REG_A0];
          return (0);
        }
        else{
          set_mem_word(state,0); /*Change state as ready.*/
          printProcessInformations(currentRunningNum);
        }
    
     }
    
      break; 
    } 
    /*This system call gives acknowledge of termination.  */
    case PROCESS_EXIT_SYSCALL:
    {
      int currentProcessNum = read_mem_word(GlobVariablesStartAddr);
      if(currentProcessNum < 0){
        cout <<"\nALL PROCESSES FINISHED SUCCESFULLY.\n" << endl;
        force_break=true;
        spim_return_value = 0;
        return (0);
      }

      break;
    }   

    case CLOSE_SYSCALL:
      {
#ifdef _WIN32
	R[REG_RES] = _close(R[REG_A0]);
#else
	R[REG_RES] = close(R[REG_A0]);
#endif
	break;
      }

    default:
      run_error ("Unknown system call: %d\n", R[REG_V0]);
      break;
    }

#ifdef _WIN32
    windowsParameterHandlingControl(1);
#endif
  return (1);
}


void
handle_exception ()
{
  if (!quiet && CP0_ExCode != ExcCode_Int)
    error ("Exception occurred at PC=0x%08x\n", CP0_EPC);

  exception_occurred = 0;
  PC = EXCEPTION_ADDR;

  switch (CP0_ExCode)
    {
    case ExcCode_Int:
      break;

    case ExcCode_AdEL:
      if (!quiet)
	error ("  Unaligned address in inst/data fetch: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_AdES:
      if (!quiet)
	error ("  Unaligned address in store: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_IBE:
      if (!quiet)
	error ("  Bad address in text read: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_DBE:
      if (!quiet)
	error ("  Bad address in data/stack read: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_Sys:
      if (!quiet)
	error ("  Error in syscall\n");
      break;

    case ExcCode_Bp:
      exception_occurred = 0;
      return;

    case ExcCode_RI:
      if (!quiet)
	error ("  Reserved instruction execution\n");
      break;

    case ExcCode_CpU:
      if (!quiet)
	error ("  Coprocessor unuable\n");
      break;

    case ExcCode_Ov:
      if (!quiet)
	error ("  Arithmetic overflow\n");
      break;

    case ExcCode_Tr:
      if (!quiet)
	error ("  Trap\n");
      break;

    case ExcCode_FPE:
      if (!quiet)
	error ("  Floating point\n");
      break;

    default:
      if (!quiet)
	error ("Unknown exception: %d\n", CP0_ExCode);
      break;
    }
}
