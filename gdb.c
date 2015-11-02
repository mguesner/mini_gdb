#include "gdb.h"
#include <stdio.h>

void do_father(t_env *e)
{
	int status;	
	waitpid(e->child, &status, WUNTRACED);
	if (!WIFSTOPPED(status))
		exit(-1);
	ptrace(PTRACE_SEIZE, e->child, 0, 0);
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
	load_sym(argv[0]);
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
	}
	line_editor_end();
	exit(ret);
}