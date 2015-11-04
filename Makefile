NAME = mini_gdb

OBJ = main.o\
	  gdb.o\
	  line_editor.o\
	  wait_event.o\
	  load_sym.o\
	  back_trace.o\
	  get_current_sym.o\
	  breakpoint.o \
	  cont.o


CC = clang -g -O3 -Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -ltermcap -o $(NAME) $(OBJ)

%.o: %.c
	$(CC) -o $@ -c $<

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean re all