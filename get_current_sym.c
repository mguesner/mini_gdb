#include "gdb.h"

char	*get_current_sym(t_env e, long addr)
{
	int		i = 0;
	while(i < e.size_sym_tab)
	{
		if (e.sym_tab[i].addr > addr)
			break;
		i++;
	}
	return e.sym_tab[i - 1].name;
}