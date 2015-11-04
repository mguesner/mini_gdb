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
#include <stdio.h>


typedef	struct 					s_line
{
	char						*line;
	int							size;
	int							allocated_size;
}								t_line;

typedef struct					s_sym
{
	long						addr;
	char						*name;
}								t_sym;

typedef struct					s_node_break
{
	unsigned int				num;
	char						opcode;							
	unsigned long long			addr;
	struct s_node_break			*next;
}								t_node_break;

typedef struct					s_lst_break
{
	t_node_break				*begin;
	t_node_break				*end;
	int							size;
}								t_lst_break;

typedef struct 					s_env
{
	int							is_running;
	int							child;
	struct user_regs_struct		regs;
	t_sym						*sym_tab;
	t_lst_break					lst_break;
	t_node_break				*current_break;
	int							size_sym_tab;
}								t_env;

void	gdb();
void	line_editor_init();
t_line	get_line();
void	line_editor_end();
int		wait_event(t_env *e, int *status);
void	load_sym(char *file, t_env *e);
void	back_trace(t_env *e);
char	*get_current_sym(t_env e, long addr);
void	breakpoint(t_env *e, char *arg);
void	cont(t_env *e);

#endif