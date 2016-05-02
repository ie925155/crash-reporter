/* 
 * File: reporter.c
 * ----------------
 * This file will eventually contain the code to print a symbolic backtrace
 * when a terminal fault is detected. Right now, it just contains a little
 * starting code to show you how to set up a signal handler.
 */



#include <string.h> // memset
#include <stdio.h>
#include <signal.h> // sigaction
#include <stdlib.h> // exit
#include <ucontext.h> // mcontext

static void SignalReceived(int signum, siginfo_t * siginfo, void *context)
{
    printf("\nProgram received signal %d (%s)\n", signum, sys_siglist[signum]);

    // These two lines get the value of EIP register at time of crash, i.e.
    // address of instruction that faulted
    const int eip_index = 14;
    void *eip = (void *)((struct ucontext *)context)->uc_mcontext.gregs[eip_index];
    // this starter code only prints the symbol addr
    // should eventually print [%p] %s (+0x%x) with addr, name, offset arguments
    printf("Faulting instruction at [%p]\n", eip);

    exit(0);  // terminate process
}

void InitReporter()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));        // init all fields to zero
    act.sa_flags = SA_SIGINFO;           // ask for 3-parameter form of the handler
    act.sa_sigaction = SignalReceived;   // set SignalReceived as callback function
    sigaction(SIGSEGV, &act, NULL);      // register as handler for segfault signal
}


