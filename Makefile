NAME	=	slr

CC	=	gcc

CFLAGS	=	-Wall -g3

SRCDIR	= 	src

OBJDIR	= 	obj

INCLUDEDIR	=	include

SRC	=	main.c		\
		slr.c		\
		closure.c	\
		parsing_table.c	\
		parsing_statements.c \


OBJ	= $(addprefix $(OBJDIR)/,$(SRC:.c=.o))

all:	$(NAME)

$(OBJDIR)/%.o:	$(SRCDIR)/%.c
			@mkdir -p $(@D)
			$(CC) -c -o $@ $< -I $(INCLUDEDIR)

$(NAME):	$(OBJ)
			 $(CC) -o $(NAME) $(OBJ) 

clean:
	rm -f $(OBJ)

fclean:	clean
	rm -f $(NAME)
	rm -rf $(OBJDIR)

re:	fclean all

.PHONY:	all re clean fclean
