#ifndef GDB_H
#define GDB_H

#include <term.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/user.h>

typedef	struct 		s_line
{
	char			*line;
	int				size;
	int				allocated_size;
}					t_line;

void	gdb();
void	line_editor_init();
t_line	get_line();
void	line_editor_end();
int		wait_event(int child, int *status);
void	load_sym(char *file);

#endif