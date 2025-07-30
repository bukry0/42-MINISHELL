NAME = minishell

CFLAGS = -Wall -Wextra -Werror

SRC =	minishell.c \
		src/builtins.c \
		src/cstm_cd.c \
		src/cstm_echo.c \
		src/cstm_env.c \
		src/cstm_exit.c \
		src/cstm_export.c \
		src/cstm_pwd.c \
		src/cstm_unset.c \
		src/envp_utils.c \
		src/exec.c \
		src/exit_minishell.c \
		src/expand_var_utils.c \
		src/expander.c \
		src/garbage_collector.c \
		src/garbage_lib.c \
		src/grbg_itoa.c \
		src/handle_redirections.c \
		src/handle_spaces.c \
		src/heredoc.c \
		src/init.c \
		src/lexer.c \
		src/list_functions.c \
		src/list_utils.c \
		src/parser.c \
		src/quotes_utils.c \
		src/signal_handler.c \
		src/split_input.c \
		src/utils.c \
		src/utils2.c

OBJ =	$(SRC:.c=.o)

LIBFT = libft/libft.a

all: $(NAME)

$(NAME): $(SRC)
	make -C ./libft -s
	cc -o $(NAME) $(CFLAGS) $(SRC) -lreadline $(LIBFT)

clean:
	make clean -C ./libft -s

fclean: clean
	make fclean -C ./libft -s
	rm -rf $(NAME)

re: fclean all

.PHONY: all, clean, fclean, re