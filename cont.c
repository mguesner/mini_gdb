#include "gdb.h"

void	cont(t_env *e)
{
	int		i = 0;
	if (e->current_break)
	{
		long tmp = ptrace(PTRACE_PEEKDATA, e->child, e->current_break->addr, NULL);
		// printf("opcode -> 0x%x at addr -> 0x%lx\n", curr->opcode, curr->addr);
		tmp = tmp - (tmp & 0b11111111) + e->current_break->opcode;
		// printf("opcode -> 0x%lx\n", tmp);
		ptrace(PTRACE_POKEDATA, e->child, e->current_break->addr, tmp);
		e->regs.rip--;
		ptrace(PTRACE_SETREGS, e->child, NULL, &e->regs);
		ptrace(PTRACE_SINGLESTEP, e->child, 0, 0);
		waitpid(e->child, &i, 0);
		tmp = ptrace(PTRACE_PEEKDATA, e->child, e->current_break->addr, NULL);
		// printf("opcode -> 0x%x at addr -> 0x%lx\n", e->current_break->opcode, e->current_break->addr);
		tmp = tmp - (tmp & 0b11111111) + 0xcc;
		// printf("opcode -> 0x%lx\n", tmp);
		ptrace(PTRACE_POKEDATA, e->child, e->current_break->addr, tmp);
		e->current_break = NULL;
	}
	wait_event(e, &i);
}