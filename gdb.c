#include "gdb.h"
#include <stdio.h>

void	insert_breakpoint(t_env *e)
{
	t_node_break *curr = e->lst_break.begin;
	while(curr)
	{
		long tmp = ptrace(PTRACE_PEEKDATA, e->child, curr->addr, NULL);
		curr->opcode = tmp & 0b11111111;
		// printf("opcode -> 0x%x at addr -> 0x%lx\n", curr->opcode, curr->addr);
		tmp = tmp - (tmp & 0b11111111) + 0xcc;
		// printf("opcode -> 0x%lx\n", tmp);
		ptrace(PTRACE_POKEDATA, e->child, curr->addr, tmp);
		curr = curr->next;
	}
}

void do_father(t_env *e)
{
	int status;	
	// long tmp;
	waitpid(e->child, &status, WUNTRACED);
	if (!WIFSTOPPED(status))
		exit(-1);
	ptrace(PTRACE_SEIZE, e->child, 0, 0);
	ptrace(PTRACE_SETOPTIONS, e->child, 0, PTRACE_O_TRACEEXEC);
	wait_event(e, &status);
	e->is_running = 1;
	insert_breakpoint(e);
	wait_event(e, &status);
}

void	do_child(char **argv)
{
	kill(getpid(), SIGSTOP);
	execvp(argv[0], argv);
	int error = errno;
    perror("execvp");
    exit(error);
}

int		start_trace(char **argv, t_env *e)
{
	e->child = fork();
	if (e->child < 0)
    {
        int error = errno;
        perror("fork");
		return error;
    }
	else if (!e->child)
        do_child(argv);
	else
        do_father(e);
    return 0;
}

void	gdb(char **argv)
{
	int		ret;
	t_env 	e;

	line_editor_init();
	load_sym(argv[0], &e);
	e.is_running = 0;
	bzero(&e.lst_break, sizeof(t_lst_break));
	while (1)
	{
		char *line = get_line().line;
		printf("\n");
		if (!strcmp(line, "r"))
			ret = start_trace(argv, &e);
		else if (!strcmp(line, "q"))
			break;
		else if (!strcmp(line, "bt"))
			back_trace(&e);
		else if (!strncmp(line, "b ", 2))
			breakpoint(&e, line + 2);
		else if (!strcmp(line, "c"))
			cont(&e);
	}
	line_editor_end();
	exit(ret);
}