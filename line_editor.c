#include "gdb.h"
#include <stdio.h>

static struct termios 	old;
static struct termios	termios_p;
static int				size_x;

static int putonterm(int c)
{
	write(1, &c, 1);
	return (1);
}

void	line_editor_init()
{
	tgetent(NULL, getenv("TERM"));
	tputs(tgetstr((char *)"im", NULL), 1, putonterm);
	tcgetattr(0, &(old));
	tcgetattr(0, &(termios_p));
	size_x = tgetnum((char *)"co");
	termios_p.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(0, TCSADRAIN, &(termios_p));
}

static void	insert_in_line(t_line *line, char c, int pos)
{
	if (line->size + 1 >= line->allocated_size)
	{
		line->line = realloc(line->line, line->allocated_size * 2);
		line->allocated_size *= 2;
	}
	int i = line->size - 1;
	while (i >= pos)
	{
		line->line[i + 1] = line->line[i];
		i--;
	}
	line->line[pos] = c;
	line->size++;
	line->line[line->size] = 0;
}

t_line	get_line()
{
	t_line	line;
	int		pos = 0;
	int		ret;
	int ch = 0;

	line.size = 0;
	line.line = malloc(50);
	line.line[0] = 0;
	line.allocated_size = 50;
	while ((ret = read(0, &ch, 4)) > 0)
	{
		switch(ch)
		{
			case 10:
				goto STOP;
			case 4479771: //LEFT
				if (pos > 0)
				{
					if (pos % size_x == 0)
					{
						tputs(tgetstr((char *)"up", NULL), 1, putonterm);
						tputs(tgoto(tgetstr((char *)"RI", NULL), 0, size_x), 1, putonterm);
					}
					else
						tputs(tgetstr((char *)"le", NULL), 1, putonterm);
					pos--;
				}
				break;
			case 4414235: //RIGHT
				if (pos < line.size)
				{
					if (pos % size_x == 0)
					{
						tputs(tgetstr((char *)"do", NULL), 1, putonterm);
						tputs(tgoto(tgetstr((char *)"LE", NULL), 0, size_x), 1, putonterm);
					}
					else
						tputs(tgetstr((char *)"nd", NULL), 1, putonterm);
					pos++;
				}
				break;
			default:
				if (ch < 128)
				{
					insert_in_line(&line, ch, pos++);
					write(1, &ch, 3);
					tputs(tgetstr((char *)"cd", NULL), 1, putonterm);
					tputs(line.line + pos, 1, putonterm);
					if (line.size % size_x == 0)
					{
						tputs(tgetstr((char *)"do", NULL), 1, putonterm);
						tputs(tgetstr((char *)"cr", NULL), 1, putonterm);
					}
					int index = line.size - pos;
					if ((line.size - index) / size_x != line.size / size_x && index != 0)
					{
						tputs(tgetstr((char *)"up", NULL), 1, putonterm);
						tputs(tgoto(tgetstr((char *)"RI", NULL), 0, size_x - index), 1, putonterm);
					}
					else if (index != 0)
						tputs(tgoto(tgetstr((char *)"LE", NULL), 0, index), 1, putonterm);
				}		
		}
		ch = 0;
	}
STOP:
	return line;
}

void	line_editor_end()
{
	tcsetattr(0, TCSAFLUSH, &(old));
	tputs(tgetstr((char *)"ei", NULL), 1, putonterm);
}