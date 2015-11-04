#include "gdb.h"

void	back_trace(t_env *e)
{
	long long	rip = e->regs.rip;
	long long	rbp = e->regs.rbp;
	int			i = 0;

	while (rip < 0x700000000000)
	{
		printf("#%d  0x%.16llx in %s()\n", i, rip, get_current_sym(*e, rip));
		rip = ptrace(PTRACE_PEEKDATA, e->child, rbp + 8, NULL);
		rbp = ptrace(PTRACE_PEEKDATA, e->child, rbp, NULL);
		i++;
	}
}