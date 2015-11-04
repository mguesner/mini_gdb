#include "gdb.h"

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

int		wait_event(t_env *e, int *status)
{
	ptrace(PTRACE_CONT, e->child, 0, 0);
	waitpid(e->child, status, 0);
	if (WIFSTOPPED(*status) && ((*status) >> 8) == (SIGTRAP | (PTRACE_EVENT_EXEC<<8)))
        return 0;
    if (WIFSTOPPED(*status) && WSTOPSIG(*status) == SIGTRAP)
    {
    	ptrace(PTRACE_GETREGS, e->child, NULL, &e->regs);
    	t_node_break	*curr = e->lst_break.begin;
    	while(curr)
    	{
    		if (curr->addr + 1 == e->regs.rip)
    			break;
    		curr = curr->next;
    	}
    	e->current_break = curr;
    	printf("Breakpoint %d, 0x%.16llx in %s()\n", curr->num + 1, e->regs.rip - 1, get_current_sym(*e, e->regs.rip));
    	return 0;
    }
    if (WIFEXITED(*status) || WIFSIGNALED(*status))
        return -1;
    printf("Program received signal %s\n", signaux[WSTOPSIG(*status)]);
	ptrace(PTRACE_GETREGS, e->child, NULL, &e->regs);
	printf("0x%.16llx in %s()\n", e->regs.rip, get_current_sym(*e, e->regs.rip));
    return WSTOPSIG(*status);
}