#include "gdb.h"
#include <stdio.h>

void do_father(int child)
{
	int status;	
	waitpid(child, &status, WUNTRACED);
	if (!WIFSTOPPED(status))
		exit(-1);
	ptrace(PTRACE_SEIZE, child, 0, 0);
	wait_event(child, &status);
}

void	do_child(char **argv)
{
	kill(getpid(), SIGSTOP);
	execvp(argv[0], argv);
	// int error = errno;
    perror("execvp");
}

int		start_trace(char **argv)
{
	int child = fork();
	if (child < 0)
    {
        int error = errno;
        perror("fork");
		return error;
    }
	else if (!child)
        do_child(argv);
	else
        do_father(child);
    return 0;
}

void	gdb(char **argv)
{
	line_editor_init();
	load_sym(argv[0]);
	int		ret;
	while (1)
	{
		char *line = get_line().line;
		printf("\n");
		if (!strcmp(line, "r"))
			ret = start_trace(argv);
		else if (!strcmp(line, "q"))
			break;
	}
	line_editor_end();
	exit(ret);
}