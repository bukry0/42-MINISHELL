/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:29:59 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:29:59 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <dirent.h> // opendir, readdir, closedir
# include <fcntl.h> // open
# include <readline/history.h> //add_history, rl_clear_history
# include <readline/readline.h> // readline, rl_...
# include <signal.h> // signal, sigaction, sigemptyset, sigaddset, kill
# include <stdio.h> // printf, perror
# include <stdlib.h> // malloc, free, exit
# include <sys/ioctl.h> // ioctl
# include <sys/types.h> // pid_t, ssize_t
# include <sys/wait.h> // wait, waitpid, wait3,4
# include <unistd.h> // write, read, close, fork, execve, pipe, dup, chdir, getcwd, access

extern int				g_exitstatus; // extern: global bir yerde tanımlandı, bu file'da da kullanacağım
									  // exitstatus: 
typedef struct s_node	t_node;
typedef struct s_prompt	t_prompt;
typedef struct s_cmddat	t_cmddat;
typedef struct s_grbg	t_grbg;

typedef struct s_prompt // programın genel durumunu tutan ana yapı
{
	char				*input_string; // kullanıcının girdiği komut satırı
	char				**commands; // split sonrası ayrılmış haldeki komutlar
	t_node				*cmd_list; // bağlı liste halinde tüm komutlar
	char				**envp; // ortam değişkenler örnğn, /home/bcili/minishell
	pid_t				pid; // sistemdeki process no, process identity
	int					stop; // çalışma kontrol flagi
	t_grbg				*grbg_lst; // çöpe gidecek (freelenecek) değişkenlerin listesi
}						t_prompt;

typedef struct s_node // komutları yönetmek için, pipe gibi birbirirnin output undan etkilenecek olanlar için
{
	t_cmddat			*data; // komut
	struct s_node		*next; // linked list için pointer
}						t_node;

typedef struct s_cmddat // komuta ait bilgileri en detaylı tutan yapı
{
	char				**full_cmd; // komut ve argümanlarının tamamı
	char				*full_path; // yürütülebilir dosya yolu
	int					infile; // dosya içine yönlendirme için <<
	int					outfile; // dosya dışına yönlendirme için >>
	t_prompt			*prompt; // ana yapıya ulaşmak için tutulan değişken
	int					file_open; // dosya açma takibi flagi
}						t_cmddat;

typedef struct s_grbg // her allocation yapılan pointer bu listeye ekleniyor ve program sonunda topluca free ediliyor.
{
	void				*ptr; // allocation yapılan pointerın adresi
	struct s_grbg		*next; // linked list oluşturmak için bir sonraki pointerın adresi
}						t_grbg;

// exit_minishell
void		free_node_list(t_node *head);
void		free_all(t_prompt *prompt);
void		exit_ms(int g_exitstatus, t_prompt *prompt);

// main
void		launch_minishell(t_prompt *prompt);
void		pipe_infile_outfile(t_node *cmd_lst);

// init
void		init_prompt_struct(t_prompt *prompt, char **envp); // prompt isimli structın içeriğini verilen envp ortam değişkenleriyle başlatır yani şuanda bulunulan dizin bilgileri vs
int			init_env(t_prompt *prompt, char **env); // program başlatılırken sistemden alınan env değişkenini shell komutlarında kullanabileceğimiz bir versiyona çevirip shell boyunca kullanacağımız prompt->envp değişkenine atar
t_cmddat	*init_struct_cmd(t_prompt *prompt);

// lexer
void		lexer(t_prompt *prompt);
char		**split_input(char *str, t_prompt *prompt);
char		**ft_split(char const *s, char c);
char		*handle_spaces(t_prompt *prompt, char *str, size_t i, int j);
char		**ft_create_substrs(t_prompt *p, char **aux, char const *s, char *set);

// quotes_utils
void		get_rid_quotes(t_prompt *prompt);
char		*get_trimmed(t_prompt *prompt, char const *s1, int squote, int dquote);
int			malloc_len(char const *str);

// parser
void		parser(t_prompt *prompt, int i, int j);
char		**fill_arr(t_prompt *main_prompt, char **prompt, int i, int len);
void		add_last_cmd_to_envp(t_prompt *p);
int			check_double_pipes(t_prompt *prompt);

// handle_redirections
int			get_type(char *str);
void		handle_redir(t_prompt *ptr, int type);
int			open_file(char **cmds, int i, int *save_fd, int io_flags[2]);
int			get_flags(int type, int file_access_type);
int			open_fd_redir(t_prompt *prompt, t_cmddat *cmd_struct, int i, int type);
int			syntax_error(t_prompt *prompt, char *token); // söz dizimi hatalarını handle edip yeni komut satırına geçiren fonksiyon

// here_doc
void		launch_heredoc(t_prompt *prompt, t_cmddat *cmd, int i);
int			get_heredoc(t_prompt *prompt, char *lim);
int			pipe_heredoc(char *content);

// builtins
int			get_builtin_nbr(t_cmddat *cmd);
int			execute_builtin(t_cmddat *cmd, int n, int forked);

// cstm_echo
int			cstm_echo(t_cmddat *cmd_data);

// cstm_pwd
int			cstm_pwd(t_cmddat *cmd_data);

// cstm_env
int			cstm_env(t_cmddat *cmd_data);

// cstm_exit
int			cstm_exit(t_cmddat *cmd_data);
int			is_only_digits(char *s);

// cstm_unset
int			cstm_unset(t_cmddat *cmd);
size_t		get_len_env(const char *s);

// cstm_cd
int			cstm_cd(t_cmddat *cmd_data);
int			go_home_dir(t_prompt *prompt);
int			go_back_dir(t_prompt *prompt);
void		modify_envp(t_prompt *prompt, char *name, char *insert);
char		*get_envp(t_prompt *prompt, char *name);

// cstm_export
int			cstm_export(t_cmddat *cmd);
int			print_export(t_cmddat *cmd);
void		print_line_export(t_cmddat *cmd, int i);
int			get_len_id(t_prompt *prompt, char *str, int msg);
int			scan_envp(t_cmddat *cmd, char *str, int id_len);

// executor
int			execute_cmds(t_prompt *prompt);
void		cls_fds(void *content);
void		run_cmd(void *content);
void		wait_update_g_exitstatus(t_prompt *prompt);
int			is_executable(t_cmddat *cmd_data);

// signal_handler
void		sigint_handler(int signum);
void		handle_sig_quit(int n);
void		signals_interactive(void);
void		handle_sig_int(int n);
void		signals_non_interactive(void);

// garbage_collector
void		*get_grbg(t_prompt *prompt, size_t nmemb, size_t size);
void		collect_grbg(t_prompt *prompt, void *new);
void		free_grbg(t_grbg *head);

// garbage_lib
char		*grbg_strdup(t_prompt *prompt, const char *s);
char		*grbg_substr(t_prompt *prompt, char const *s, unsigned int start, size_t len);
char		*grbg_itoa(t_prompt *prompt, int n);
char		*grbg_strjoin(t_prompt *prompt, char const *s1, char const *s2);

// list_functions
void		cstm_lstiter(t_node *lst, void (*f)(void *));
t_node		*cstm_lstlast(t_node *lst);
int			cstm_lstsize(t_node *lst);
void		cstm_lstclear(t_node **lst, void (*del)(void *));
void		cstm_lstdelone(t_node *lst, void (*del)(void *));

// utils
size_t		get_len_arr(char **array); // array uzunluğunu bulan fonksiyon
void		print_err_msg(char *cmd, char *msg); // sadece komut varsa yazılacak hata mesajının düzenlendiği fonksiyon
int			print_err_msg_lng(char *cmd, char *msg, char *arg); // hem komut hem argüman bulunan hata mesajlarını yazdırmak için kullanılan fonksiyon
int			ft_isspace(int c); // gönderilen karakterin space ya da tab benzeri atlanması gereken karakterler mi olduğunu tespit eder
char		**del_str(char **array, int pos, int count); // string dizisinden seçilen bir stringin çıkartılmasını sağlayan fonksiyon pos: silinecek stringin arraydeki indexi, count: dizideki toplam string sayısı
char		**add_str_to_arr(t_prompt *prompt, char **arr, char *str); // verilen string dizisinin sonuna yeni bir string daha ekleyen fonksiyon
char		*add_to_str(t_prompt *prompt, char **str, char *add); // verilen string sonuna add değişkeni içindeki stringi de ekleyen fonksiyon

// list_utils
void		add_node_to_list(t_prompt *prompt, t_node **head, t_cmddat *data);
int			ft_listsize(t_node *lst);

// envp_utils
char		*get_path_cmds(t_cmddat *cmd, char **ev);
char		*get_path(char *cmd, char **ev, size_t i);
void		free_split(char **strs);

// expander
char		**expander(t_prompt *prompt, char **str, char **ev);
char		*handle_g_exitstatus(t_prompt *prompt, int i, char *str, char *sub_str);

// expand_var_utils
char		*create_sub_var(char *str, size_t i, char **ev, ssize_t len);
ssize_t		get_len_var(char *str, int i);
char		*create_sub(char *str, size_t i, char *nb, ssize_t len);
char		*expand_var(t_prompt *prompt, char *str, char **ev, int i);
char		*handle_expansion(t_prompt *prompt, char *str, int q[4], char *sub_str);

#endif