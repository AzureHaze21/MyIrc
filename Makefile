NAME	= server

SRC	=src/channel.c	\
        src/cleanup.c	\
        src/commands2.c	\
        src/commands3.c	\
        src/commands.c	\
        src/irc_server.c \
        src/join.c	\
        src/loop.c	\
        src/main.c	\
        src/msg.c	\
        src/names.c	\
        src/notify.c	\
        src/time.c	\
        src/topic.c	\
        src/user.c	\
        src/utils.c

OBJS	= $(SRC:.c=.o)

CC	= gcc

CFLAGS	= -W -Wall -Wextra -I./inc

RM	= rm -f
ECHO	= echo -e

all	: $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(INC) $(CFLAGS) -o $(NAME) $(OBJS)
	@$(ECHO) '\033[0;32m> Compiled\033[0m'

clean	:
	$(RM) $(OBJS)
	$(RM) *~
	$(RM) \#*#
	@$(ECHO) '\033[0;31m> Directory cleaned\033[0m'

fclean	: clean
	$(RM) $(NAME)
	@$(ECHO) '\033[0;31m> Remove executable\033[0m'

re	: fclean all

.PHONY	: all clean fclean re
