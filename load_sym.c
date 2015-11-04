#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "gdb.h"

void	swap(t_sym *tab1, t_sym *tab2)
{
	long tmp1;
	char	*tmp2;
	tmp1 = tab1->addr;
	tmp2 = tab1->name;
	tab1->addr = tab2->addr;
	tab1->name = tab2->name;
	tab2->addr = tmp1;
	tab2->name = tmp2;
}

void	sort_array(t_sym *array, int begin, int end)
{
	if (end > begin) {
      long pivot = (array + begin)->addr;
      int l = begin + 1;
      int r = end;
      while(l < r) {
         if ((array + l)->addr <= pivot)
            l += 1;
         else if ((array + r)->addr > pivot)
            r -= 1;
         else if ( l < r )
            swap(array+l, array+r);
      }
      l -= 1;
      swap(array+begin, array+l);
      sort_array(array, begin, l);
      sort_array(array, r, end);
   }
}

void	load_sym(char *file, t_env *e)
{
	int fd = open(file, O_RDONLY);
	struct stat buf;
	fstat(fd, &buf);
	void *file_start = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, fd, 0);
	Elf64_Ehdr *header = (Elf64_Ehdr*)file_start;
	// printf("Magic:\t%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n\
	// 	Type -> %u\n\
	// 	Entry point adress: 0x%lx\n"
	// 	, header->e_ident[0], header->e_ident[1], header->e_ident[2]
	// 	, header->e_ident[3], header->e_ident[4], header->e_ident[5]
	// 	, header->e_ident[6], header->e_ident[7], header->e_ident[8]
	// 	, header->e_ident[9], header->e_ident[10], header->e_ident[11]
	// 	, header->e_ident[12], header->e_ident[13], header->e_ident[14]
	// 	, header->e_ident[15], header->e_type, header->e_entry);
	Elf64_Shdr *section = (Elf64_Shdr*)(file_start + header->e_shoff + 26 * sizeof(Elf64_Shdr));
	char *str_tbl = (char *)(file_start + section->sh_offset);
	// printf("Name: %u, Addr: %lx\n\n", section->sh_name, section->sh_addr);
	// char *str_tbl = file_start + ((Elf64_Shdr *)(file_start + header->e_shstrndx))->sh_offset;
	int off = header->e_shoff;
	Elf64_Sym *sym ;
	int sym_size;
	char *sym_str_tbl;
	for(int i = 0; i < header->e_shnum; i++)
	{
		Elf64_Shdr *section = (Elf64_Shdr*)(file_start + off);
		// printf("Name: %s, Addr: 0x%lx\n", str_tbl + section->sh_name, section->sh_addr);
		if (section->sh_type == SHT_SYMTAB)
		{
			sym = (Elf64_Sym *)(file_start + section->sh_offset);
			sym_size = section->sh_size;
		}
		if (!strcmp(str_tbl + section->sh_name, ".strtab"));
		{
			sym_str_tbl = (char *)(file_start + section->sh_offset);
			// write(1, sym_str_tbl, section->sh_size);
		}
		off += sizeof(Elf64_Shdr);
	}
	Elf64_Sym *curr_sym = sym;
	e->sym_tab = malloc(sym_size / sizeof(Elf64_Sym) * sizeof(t_sym));
	int i = 0;
	while((void *)curr_sym < ((void *)sym) + sym_size)
	{
		if (ELF64_ST_TYPE(curr_sym->st_info) == STT_FUNC)
		{
			// printf("Name: %s, Addr: 0x%lx, bind: %d\n", sym_str_tbl + curr_sym->st_name
			// 	, curr_sym->st_value, ELF64_ST_BIND(curr_sym->st_info));
			e->sym_tab[i].addr = curr_sym->st_value;
			e->sym_tab[i].name = sym_str_tbl + curr_sym->st_name;
			i++;
		}
		curr_sym = curr_sym + 1;
	}
	e->size_sym_tab = i;
	sort_array(e->sym_tab, 0, e->size_sym_tab);
	i = 0;
	while (i < e->size_sym_tab)
	{
		printf("Name: %s, Addr: 0x%lx\n", e->sym_tab[i].name , e->sym_tab[i].addr );
		i++;
	}
}