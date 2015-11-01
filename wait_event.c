#include "gdb.h"
#include <stdio.h>

char *signaux[] =
{
    [0] = "NONE",
    [1] = "SIGHUP",
    [2] = "SIGINT",
    [3] = "SIGQUIT",
    [4] = "SIGILL",
    [5] = "SIGTRAP",
    [6] = "SIGABRT",
    [7] = "SIGBUS",
    [8] = "SIGFPE",
    [9] = "SIGKILL",
    [10] = "SIGUSR1",
    [11] = "SIGSEGV",
    [12] = "SIGUSR2",
    [13] = "SIGPIPE",
    [14] = "SIGALRM",
    [15] = "SIGTERM",
    [16] = "SIGSTKFLT",
    [17] = "SIGCHLD",
    [18] = "SIGCONT",
    [19] = "SIGSTOP",
    [20] = "SIGTSTP",
    [21] = "SIGTTIN",
    [22] = "SIGTTOU",
    [23] = "SIGURG",
    [24] = "SIGXCPU",
    [25] = "SIGXFSZ",
    [26] = "SIGVTALRM",
    [27] = "SIGPROF",
    [28] = "SIGWINCH",
    [29] = "SIGIO",
    [30] = "SIGPWR",
    [31] = "SIGSYS"
};

int		wait_event(int child, int *status)
{
	ptrace(PTRACE_CONT, child, 0, 0);
	waitpid(child, status, 0);
	if (WIFSTOPPED(*status) && WSTOPSIG(*status) & 0x80)
        return 0;
    if (WIFEXITED(*status) || WIFSIGNALED(*status))
        return -1;
    printf("Program received signal %s\n", signaux[WSTOPSIG(*status)]);
    struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS, child, NULL, &regs);
	printf("0x%.16llx\n", regs.rip);
    return WSTOPSIG(*status);
}