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
#define SIZE 100
using namespace std; 

/*Enum for process states. */
enum ProcessState{ 
  ready=0,
  running=1,
  blocked=2
};
/*Process Table. */
struct ProcessTable{
  mem_addr pc;
  int pid;
  int ppid;
  char processName[SIZE];
  mem_addr text_segAddress;
  mem_addr data_segAddress;
  mem_addr stack_segAddress;
  reg_word registers[R_LENGTH];
  reg_word hi;
  reg_word lo;
  reg_word ccr[4][32];
  reg_word cpr[4][32];
  ProcessState procState;

};
ProcessTable *procTable[SIZE]={};
/*Global variables. */
int processCount=0;
int currentRunning=0;
int waitpidReturn;
int waitpidControl=0;

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

/* Prints process table of current process  on screen. */
void printProcessInformations(int currentRunning){
    cout<< "\n\n-------IN TIMER INTERRUPT HANDLER --------\n\n";
    cout<<"Process Name:"<<(*procTable[currentRunning]).processName<<endl;
    cout<< hex <<"Program Counter:"<<(*procTable[currentRunning]).pc<<endl;
    cout<< dec << "Process Id:"<<(*procTable[currentRunning]).pid<<endl;
    cout << dec <<"Parent Process Id:"<<(*procTable[currentRunning]).ppid<<endl;
    cout<<hex<<"Data Pointer Address:"<<(*procTable[currentRunning]).data_segAddress<<endl;
    cout<<hex<<"Stack Pointer Address:"<<(*procTable[currentRunning]).stack_segAddress<<endl;
    cout<<hex<<"End of Text Pointer Address:"<<(*procTable[currentRunning]).text_segAddress<<endl;
    if((*procTable[currentRunning]).procState==running)
      cout<<"Process State:"<<" RUNNING "<<endl<< endl;
    else {
      cout<<"Process State:"<<" READY "<<endl<< endl;
    }
      
}
/*Interrupt handler.That handler provide to save current process informations  and then to pass next process using Round Robin scheduling.*/
void SPIM_timerHandler()
{ 
    /*This if condition controls current process deleted or not. */
    if( procTable[currentRunning]!=NULL){
      int i=0,j=0;
      /*Below operations to save  computer memory informations to current process. */
      for(i=0;i<32;++i)
          (*procTable[currentRunning]).registers[i]=R[i];  
      for(i=0;i<4;i++){
        for(j=0;j<32;j++){
          (*procTable[currentRunning]).ccr[i][j]=CCR[i][j];
          (*procTable[currentRunning]).cpr[i][j]=CPR[i][j];
        }  
      }
      (*procTable[currentRunning]).pc=PC;
      (*procTable[currentRunning]).hi=HI;
      (*procTable[currentRunning]).lo=LO;


      string s =  (*procTable[currentRunning]).processName;
      if(s.compare("Collatz.asm")==0  && waitpidControl==0){
        (*procTable[currentRunning]).procState=running;
        /*Prints process informations on screen. */
        printProcessInformations(currentRunning);
      }
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

	read_input (str, 2);
	if (*str == '\0') *str = '\n';      /* makes xspim = spim */
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
    /*This system call  creates a init process  and initializes process table. */  
    case INITIALIZE_SYSCALL:
    {
      int i=0,j=0;
      procTable[0]=new ProcessTable();  /*creates init process. */
      mem_addr addressText,addressData; 
      addressText=starting_address();
      /*This loop to find end of text segment address. */
      while(text_seg[i]!=0){
          addressText=addressText+4;
          i=i+1;
      }
      addressData=current_data_pc();  /*Data segment address. */

      /*Below loops for record computer registers in process table for init process. */
      for(i=0;i<32;++i)
          (*procTable[0]).registers[i]=R[i];  
      for(i=0;i<4;i++){
        for(j=0;j<32;j++){
          (*procTable[0]).ccr[i][j]=CCR[i][j];
          (*procTable[0]).cpr[i][j]=CPR[i][j];
        }  
      }
      /*Assigning other informations to process table of init process. */
      (*procTable[0]).pc=PC;
      (*procTable[0]).pid=0; /*Process id of init process is 0. */
      R[REG_RES]=0;
      (*procTable[0]).ppid=-1;
      (*procTable[0]).hi=HI;
      (*procTable[0]).lo=LO;
      strcpy((*procTable[0]).processName,"init");
      (*procTable[0]).procState=running;
      (*procTable[0]).text_segAddress=addressText;
      (*procTable[0]).data_segAddress=addressData;
      (*procTable[0]).stack_segAddress=R[REG_SP];

      processCount ++; /*Increase process number. */
      currentRunning=0; /*current running process is init process. */

      break;
    }    
    /*This is fork system call that creates a new process that is copy of parent process. */
    case FORK_SYSCALL:
    {
      int i=0,j=0;
      procTable[processCount]=new ProcessTable(); /*Creates a new process. */
      mem_addr addressText,addressData; 
      addressText=starting_address();
      /*This loop to find end of text segment address. */
      while(text_seg[i]!=0){
          addressText=addressText+4;
          i=i+1;
      }
      addressData=current_data_pc();  /*Data segment address. */

      /*Below loops for record registers of parent process in process table for new created fork process that is copy of parent. */
      for(i=0;i<32;++i)
           (*procTable[processCount]).registers[i]=R[i];  
      for(i=0;i<4;i++){
        for(j=0;j<32;j++){
          (*procTable[processCount]).ccr[i][j]=CCR[i][j];
           (*procTable[processCount]).cpr[i][j]=CPR[i][j];
        }  
      }
      /*Assigning other informations to process table of new created fork process that is copy of parent. */
      (*procTable[processCount]).pc=PC;
      (*procTable[processCount]).pid=processCount;
      (*procTable[processCount]).ppid=processCount-1;
      (*procTable[processCount]).hi=HI;
      (*procTable[processCount]).lo=LO;
      strcpy( (*procTable[processCount]).processName,"childProcess"); 
      (*procTable[processCount]).procState=ready;
      (*procTable[processCount]).text_segAddress=addressText;
      (*procTable[processCount]).data_segAddress=addressData;
      (*procTable[processCount]).stack_segAddress=R[REG_SP];
    
      processCount ++;    /*Increase process number. */  
    
      break;
    }
    /*This is execve system call that replaces process's image core by given file. */
    case EXECVE_SYSCALL:
    {
      mem_addr addressText,addressData;
      int i=0,j=0;
      int execveProcess=processCount-1;
      addressText=starting_address();
      /*This loop to find end of text segment address. */
      while(text_seg[i]!=0){
        addressText=addressText+4;
        i=i+1;
      }
      string file((char*)mem_reference (R[REG_A0])); /*file name */
     
      read_assembly_file(const_cast<char*>(file.c_str())); /*reads given file. */

      /*Replaces process's registers by registers of given file. */
      for(i=0;i<32;++i)
          (*procTable[execveProcess]).registers[i]=R[i];  
      for(i=0;i<4;i++){
        for(j=0;j<32;j++){
          (*procTable[execveProcess]).ccr[i][j]=CCR[i][j];
          (*procTable[execveProcess]).cpr[i][j]=CPR[i][j];
        }  
      }
      /*Assigning other informations to change process's image core according to given file.*/
      addressData=current_data_pc();
      (*procTable[execveProcess]).pc=addressText;
      (*procTable[execveProcess]).pid=processCount-1;
      R[REG_RES]=(*procTable[execveProcess]).pid;
      (*procTable[execveProcess]).ppid=processCount-2;
      (*procTable[execveProcess]).hi=HI;
      (*procTable[execveProcess]).lo=LO;
      strcpy((*procTable[execveProcess]).processName,const_cast<char*>(file.c_str())); 
      (*procTable[execveProcess]).procState=ready;
      (*procTable[execveProcess]).text_segAddress=current_text_pc();
      (*procTable[execveProcess]).data_segAddress=addressData;
      (*procTable[execveProcess]).stack_segAddress=R[REG_SP];

      break;
    }
    /*This is waitpid system call that provide to wait until end of given  process. */
    case WAITPID_SYSCALL :
    {
      waitpidControl=0;
      waitpidReturn=R[REG_A0];
      break;
    } 
    /*This system call is using end of a file.That deletes process from process table.*/
  case END_OF_PROCESS_SYSCALL:
    { 
      if(procTable[currentRunning]!=NULL){
        /*Deletes the process from process table and decreases process count.*/
        delete procTable[currentRunning];
        procTable[currentRunning]=NULL;
        processCount=processCount-1;
        currentRunning=currentRunning+1;
        /*If all processes finished,terminate program.*/
        if(processCount==0){
          cout << "\nALL PROCESSES FINISHED SUCCESFULLY.\n" << endl;
          force_break=true;
          spim_return_value = R[REG_A0];
          return (0);
        }
        /*If there are process,prints informations of next process. */
        else{
          (*procTable[currentRunning]).procState=ready;
          printProcessInformations(currentRunning);
        }
    
      }
    
      break; 
    } 
    /*This is a random number generator syscall.That creates random number that has interval of 1-3 */
    case RANDOM_GENERATOR_SYSCALL:
    {
      srand (time(NULL));
      int random= rand() % 3 + 1;
      R[REG_RES]=random;

      break;
    }  
    /*This system call gives acknowledge of termination.  */
    case PROCESS_EXIT_SYSCALL:
    {
      if(processCount < 0){
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
