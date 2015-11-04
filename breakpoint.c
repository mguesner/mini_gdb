#include "gdb.h"

void	add_node(t_lst_break *lst, t_node_break *node)
{
	if (!lst->size)
	{
		lst->begin = node;
		lst->end = node;
		lst->size = 1;
	}
	else
	{
		lst->end->next = node;
		lst->end = node;
		lst->size++;
	}
}

void	breakpoint(t_env *e, char *arg)
{
	//trim arg
	int		i = 0;
	long	addr = 0;
	if (arg[0] == '*')
		addr = strtol(arg, NULL, 16);
	else
	{
		while (i < e->size_sym_tab)
		{
			if (e->sym_tab[i].name && !strcmp(e->sym_tab[i].name, arg))
			{
				addr = e->sym_tab[i].addr + 4;
				break;
			}
			i++;
		}
	}
	t_node_break	*node;
	node = malloc(sizeof(t_node_break));
	node->num = e->lst_break.size;
	node->addr = addr;
	node->next = NULL;
	if (e->is_running)
	{
		long tmp = ptrace(PTRACE_PEEKDATA, e->child, addr, NULL);
		printf("opcode -> 0x%lx\n", tmp);
		node->opcode = tmp & 0b11111111;
		tmp = tmp - (tmp & 0b11111111) + 0xcc;
		printf("opcode -> 0x%lx\n", tmp);
		ptrace(PTRACE_POKEDATA, e->child, addr, tmp);
	}
	add_node(&e->lst_break, node);
	printf("breakpoint %d at 0x%llx\n", node->num + 1, node->addr);
}