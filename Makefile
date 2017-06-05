SRC	=	main.c		\
		slr.c		\
		closure.c	\
		parsing_table.c	\
		parsing_statements.c \

NAME	=	slr

CC	=	gcc

CFLAGS	=	-Wall -g3

OBJ	=	$(SRC:.c=.o)

all:	$(NAME)

$(NAME):	$(OBJ)
		$(CC) -o $(NAME) $(OBJ) 

clean:
	rm -f $(OBJ)

fclean:	clean
	rm -f $(NAME)

re:	fclean all

.PHONY:	all re clean fclean
