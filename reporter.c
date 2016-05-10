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
#include "symbols.h"

static void SignalReceived(int signum, siginfo_t * siginfo, void *context)
{
    printf("\nProgram received signal %d (%s)\n", signum, sys_siglist[signum]);

    // These two lines get the value of RIP register at time of crash, i.e.
    // address of instruction that faulted
    const int rip_index = 16;
    const int rbp_index = 10;
    void *rip = (void *)((struct ucontext *)context)->uc_mcontext.gregs[rip_index];
    void *rbp = (void *)((struct ucontext *)context)->uc_mcontext.gregs[rbp_index];
    // this starter code only prints the symbol addr
    // should eventually print [%p] %s (+0x%x) with addr, name, offset arguments

    char buf[256] = {0x0};
    sprintf(buf, "0x%llx", (long long int)rip);
    long long int offset;
    char* symbol = (char*)SearchSymbol(buf, &offset);
    printf("Faulting instruction at [%p] %s (+0x%x)\n", rip, symbol, offset);
    memset(buf, 0x00, 256);
    sprintf(buf, "0x%llx", (long long int) *((int64_t*)rbp+1));
    printf("buf [%s]\n", buf);
    SearchSymbol(buf, &offset);

    exit(0);  // terminate process
}

void InitReporter()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));        // init all fields to zero
    act.sa_flags = SA_SIGINFO;           // ask for 3-parameter form of the handler
    act.sa_sigaction = SignalReceived;   // set SignalReceived as callback function
    sigaction(SIGABRT, &act, NULL);
    sigaction(SIGBUS, &act, NULL);
    sigaction(SIGFPE, &act, NULL);
    sigaction(SIGILL, &act, NULL);
    sigaction(SIGSEGV, &act, NULL);      // register as handler for segfault signal
    ObjectFileOpen("/proc/self/exe");
}
