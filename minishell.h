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
void		free_node_list(t_node *head); // gönderilen linked list içindeki bütün değişkenlerin hafızada kullandıkları alanları serbest bırakan fonksiyon
void		free_all(t_prompt *prompt); // program kapanmadan öncr bütün değişkenlerin alanlarını serbest bırakacak fonksiyon
void		exit_ms(int g_exitstatus, t_prompt *prompt); // programı kapatıp çıkan fonksiyon

// main
void		launch_minishell(t_prompt *prompt); // shell programımızın ana çalışma döngüsünü  içeren fonksiyon
void		pipe_infile_outfile(t_node *cmd_lst); // komut listesindeki her komutu bir sonraki ile | (pipe) üzerinden bağlamak

// init
void		init_prompt_struct(t_prompt *prompt, char **envp); // prompt isimli structın içeriğini verilen envp ortam değişkenleriyle başlatır yani şuanda bulunulan dizin bilgileri vs
int			init_env(t_prompt *prompt, char **env); // program başlatılırken sistemden alınan env değişkenini shell komutlarında kullanabileceğimiz bir versiyona çevirip shell boyunca kullanacağımız prompt->envp değişkenine atar
t_cmddat	*init_struct_cmd(t_prompt *prompt); // komut yapısını tutan değişkenlerin başlangıç değerlerini veren fonksiyon

// lexer
void		lexer(t_prompt *prompt); // kullanıcının girdiği komut satırının alınıp analiz edildiği ve bölündüğü fonksiyon
char		**split_input(char *str, t_prompt *prompt); // komut satırında girilen stringi spacelere ve tırnaklara göre anlamlı parçalara ayıran foksiyon
char		**ft_split(char const *s, char c);
char		*handle_spaces(t_prompt *prompt, char *str, size_t i, int j); // space düzenlenmesi ihtimali olan karakterleri space düzenlemesi yapan fonksiyonlara aktaran fonksiyon
char		**ft_create_substrs(t_prompt *p, char **aux, char const *s, char *set); // set karakterine göre ve tırnaklara göre girilen komut satırını bölen fonksiyon

// quotes_utils
void		get_rid_quotes(t_prompt *prompt); // artık input token edildiği için artık zaten parçalanmış stringlerde bulunan tırnakları temizlememizi sağlayan fonksiyon
char		*get_trimmed(t_prompt *prompt, char const *s1, int squote, int dquote); // gönderilen stringde tırnak karakterleri dışındaki karakterleri kopyalayıp tırnaksız halini döndüren fonksiyon
int			malloc_len(char const *str); // gönderilen stringde kaç tane tırnak işareti silineceğininin sayısını döndüren fonksiyon

// parser
void		parser(t_prompt *prompt, int i, int j); // lexer ile spacelerle bölünüp birleştirilen komut dizisini anlamlı parçalar halinde bölen fonksiyon mesela pipe bulursa pipe öncesi olan komutu ayrı pipe sonrası olan komutu ayrı çalıştırmak gerekir bu tarz durumlarda işi kolaylaştırmaya yarar
char		**fill_arr(t_prompt *main_prompt, char **prompt, int i, int len); // başlangıç indexi ve uzunluğu gönderilmiş komut parçalarını beraber çalışacakları tespit edildiği için bir düğüm içine sırasıyla yerleştiren fonksiyon
void		add_last_cmd_to_envp(t_prompt *p); // son komuta ilerle ve o komutun ilk indexteki içeriğini ortam değişkenlerine "_=" olarak kaydeden fonksiyon
int			check_double_pipes(t_prompt *prompt); // girilen pipe shell komutlarında veya anlamına gelen || çift halde mi girilmiş yoksa farklı işlevi olan | olarak mı girilmiş bunun kontrolü yapılır

// handle_redirections
int			get_type(char *str); // gönderilen komut stringinde redirectionları ve tiplerini tespit eden fonksiyon
void		handle_redir(t_prompt *ptr, int type); // redirection komutlarının tespitini ve sonrasındaki yapılacak işlemleri ayarlayan fonksiyon
int			open_file(char **cmds, int i, int *save_fd, int io_flags[2]); // redirection yapılmış bir dosyanın içerisine istenilen işlem için erişilebilmesi için open ile açan fonksiyon
int			get_flags(int type, int file_access_type); // input ya da output redirectionları var mı bunun tespitini yapan fonksiyon
int			open_fd_redir(t_prompt *prompt, t_cmddat *cmd_struct, int i, int type); // istenilen redirection a uygun modda istenilen dosyaların açılmasını sağlayan fonksiyon
int			syntax_error(t_prompt *prompt, char *token); // söz dizimi hatalarını handle edip yeni komut satırını başlatan fonksiyon

// here_doc
void		launch_heredoc(t_prompt *prompt, t_cmddat *cmd, int i);
int			get_heredoc(t_prompt *prompt, char *lim);
int			pipe_heredoc(char *content);

// builtins
int			get_builtin_nbr(t_cmddat *cmd); // girilen kodun karakterlerini kontrol eder ve hangi kodun builltin olup olmadığına göre true veya false değerler döndürür ve eğer komut builtins ise hangi builtins olduğunu farklı rakamlar göndererek belirtir örneğin; echo:1, cd:2, pwd:3, export:4, unset:5, env:6, exit:7 döndüren fonksiyon
int			execute_builtin(t_cmddat *cmd, int n, int forked); // hangi builtin komutu olduğu numaralandırılarak n ile gönderilen komutu istenilen şekilde çalıştıracak fonksiyona gönderen fonksiyon

// cstm_echo
int			cstm_echo(t_cmddat *cmd_data); // echo (yazma) komutunun işlevini yapan fonksiyon

// cstm_pwd
int			cstm_pwd(t_cmddat *cmd_data); // pwd (bulunduğu dizini yazma) komutunun işlevini yapan fonksiyon

// cstm_env
int			cstm_env(t_cmddat *cmd_data); // env komutunun işlevini manuel olarak yapacak fonksiyon

// cstm_exit
int			cstm_exit(t_cmddat *cmd_data); // exit komutu girildiğinde programdan manuel olarak çıkış yapacak fonksiyon
int			is_only_digits(char *s); // gönderilen string sadece nümerik karakterlerden mi oluşuyor diye kontrol eden fonksiyon 0: false, digit değil, 1: true, digit

// cstm_unset
int			cstm_unset(t_cmddat *cmd); // unset (env değişkeni silme) komutunun işlevini manuel olarak yapan fonksiyon
size_t		get_len_env(const char *s); // key-value olarak kaydedilen env (ortam değişkenlerinin) key değerinin kaç karakter olduğunu sayan fonksiyon

// cstm_cd
int			cstm_cd(t_cmddat *cmd_data); // cd (dizin değiştirme) komutunun yapacağı işi manuel yapacak fonksiyon
int			go_home_dir(t_prompt *prompt); // HOME dizinine geçmek için cd ya da cd ~ komutları girildiyse HOME dizinine geçen fonksiyon
int			go_back_dir(t_prompt *prompt);// cd - komutu girilirse history (OLDPWD) den bir önceki dizine geri geçilmesini sağlayan fonksiyon
void		modify_envp(t_prompt *prompt, char *name, char *insert); // verilen environment değişkenini istenilen yeni değeriyle güncelleyen fonksiyon
char		*get_envp(t_prompt *prompt, char *name); // verilen isimdeki environment değişkeninin değerini döndüren fonksiyon

// cstm_export
int			cstm_export(t_cmddat *cmd); // export (yeni env değişkeni tanımlama) komutunun işini manuel yapacak fonksiyon
int			print_export(t_cmddat *cmd); // sadece export olarak girildiyse komut bütün export edilmiş değişkenleri ekrana yazdıran fonksiyon
void		print_line_export(t_cmddat *cmd, int i); // sadece export komutu girildiğinde ekrana yazdırılacak env değerinin 1 satırını doğru formatta yazdıran fonksiyon
int			get_len_id(t_prompt *prompt, char *str, int msg); // gönderilen env değerinin key kısmının uzunluğunu döndürecek ve key değeri isim formatına uygun mu kontrolü yapacak fonksiyon
int			scan_envp(t_cmddat *cmd, char *str, int id_len); // eklenmek için gönderilen değişkeni env de uygun yere ekler (zaten varsa günceller yoksa yeni env olarak ekler)

// executor
int			execute_cmds(t_prompt *prompt); // kod çalıştırma işlemi başlamadan önce bazı analizler sonucu ön hazırlıkları da yapılmış komutların işlerinin başlaması için ana fonksiyonlara yönlendiren fonksiyon
void		cls_fds(void *content); // gönderilen komut içeriğindeki bilgilerden açık olan dosya olup olmadığını kontrol eden ve eğer açık dosya varsa kaptan fonksiyon
void		run_cmd(void *content); // builtin olmayan ya da birden fazla iç içe komut içeren komut dizilerinin çalışmasını sağlayan fonksiyonlara yönlendiren fonksiyon
void		wait_update_exitstatus(t_prompt *prompt); // bütün child processlerin beklendiği ve biten child processlere göre exit code ların atandığı fonksiyon
int			is_executable(t_cmddat *cmd_data); // gönderilen komut bulunduğu dizinde çalıştırılabilir mi bunu kontrol eden fonksiyon

// signal_handler
void		sigint_handler(int signum);
void		handle_sig_quit(int n); // bir komutun çalışması esnasında yarıda kapatmak için sinyal gönderilirse döngüleri durdurup yeni satır başlatacak fonksiyon
void		signals_interactive(void); // komut satırında henüz komut girilmemişken CTRL+C ye basıldığında shell çökmesin diye kendi handle ettiğimiz fonksiyon
void		handle_sig_int(int n); // zaten yeni bir komut satırındayken yani fork() exec() gibi işlemler aktif değilken gönderilen CTRL+C sinyalini ele alan fonksiyon
void		signals_non_interactive(void); // çalışan bir komut varken yani aktif bir fork() ve execve() varken girilen sinyallerin handle edilmesini sağlayan fonksiyon

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
void		cstm_lstiter(t_node *lst, void (*f)(void *)); // gönderilen listenin bütün elemanlarına f fonksiyonunu sırasıyla uygula
t_node		*cstm_lstlast(t_node *lst); // gönderilen listenin son elemanına kadar ilerleyip son elemanını döndüren fonksiyon
int			cstm_lstsize(t_node *lst); // gönderilen linked list in kaç elemanı olduğunu döndüren fonksiyon
void		cstm_lstclear(t_node **lst, void (*del)(void *)); // gönderilen listenin tamamını silen fonksiyon
void		cstm_lstdelone(t_node *lst, void (*del)(void *)); // gönderilen düğümü silen fonksiyon

// utils
size_t		get_len_arr(char **array); // array uzunluğunu bulan fonksiyon
void		print_err_msg(char *cmd, char *msg); // sadece komut varsa yazılacak hata mesajının düzenlendiği fonksiyon
int			print_err_msg_lng(char *cmd, char *msg, char *arg); // hem komut hem argüman bulunan hata mesajlarını yazdırmak için kullanılan fonksiyon
int			ft_isspace(int c); // gönderilen karakterin space ya da tab benzeri atlanması gereken karakterler mi olduğunu tespit eder
char		**del_str(char **array, int pos, int count); // string dizisinden seçilen bir stringin çıkartılmasını sağlayan fonksiyon pos: silinecek stringin arraydeki indexi, count: dizideki toplam string sayısı
char		**add_str_to_arr(t_prompt *prompt, char **arr, char *str); // verilen string dizisinin sonuna yeni bir string daha ekleyen fonksiyon
char		*add_to_str(t_prompt *prompt, char **str, char *add); // verilen string sonuna add değişkeni içindeki stringi de ekleyen fonksiyon

// list_utils
void		add_node_to_list(t_prompt *prompt, t_node **head, t_cmddat *data); // verilen linked listeye yeni bir düğüm daha ekleyen fonksiyon
int			ft_listsize(t_node *lst); // gönderilen linked list in uzunluğunu hesaplar

// envp_utils
char		*get_path_cmds(t_cmddat *cmd, char **ev); // verilen komutun çalıştırılabilir tam yolunu bulmak, örn: "ls" girildiğinde /bin/ls şeklinde döndürmeyi sağlayan fonksiyon
char		*get_path(char *cmd, char **ev, size_t i); // prompt süreci boyunca o an bulunduğumuz dosya yolunda gönderdiğimiz komuta izin olup olmadığını tespit edecek fonksiyon
void		free_split(char **strs); // gönderilen string dizisine ayrılan alanları boşalt

// expander
char		**expander(t_prompt *prompt, char **str, char **ev); // str içindeki anlamlı parçalara bölünmüş kodların içinde expanded değişkenlerin ($ ile başlayan bash değişkenler) sistemdeki değerlerini yerine yazıp güncelleyen fonksiyon
char		*handle_g_exitstatus(t_prompt *prompt, int i, char *str, char *sub_str); // eğer komut satırında $? değişkeni tespit edilirse onun alması gereken global g_exitstatus değerini yerine yazıp gönderen fonksiyon

// expand_var_utils
char		*create_sub_var(char *str, size_t i, char **ev, ssize_t len); // stringde tespit edilen değişkenin eğer atanmış bir değeri varsa onu yerine koyup stringin geri kalanıyla birleştirip gönderen fonksiyon
ssize_t		get_len_var(char *str, int i); // verilen str stringinde i. indexten sonrasındaki karakterleri sayan; space, str sonu ya da tırnak görene kadar ve bu boyutu döndüren fonksiyon
char		*create_sub(char *str, size_t i, char *nb, ssize_t len); // g_exitstatus değerini $? komutu yerine yazan fonksiyon
char		*expand_var(t_prompt *prompt, char *str, char **ev, int i); // stringin tamamında tespit edilen değişkenleri eğer değeri varsa değeri verilip yoksa eski haliyle aynısının döndürüldüğü fonksiyon
char		*handle_expansion(t_prompt *prompt, char *str, int q[4], char *sub_str); // $ karakterini bulan ve sonrasında kayda değer bir değişken ismi vs varsa ona değerini aktaracak fonksiyonları çalıştıran ve değerlerin yerine yazıldığı son halini döndüren fonksiyon

#endif