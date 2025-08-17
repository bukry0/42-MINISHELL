/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:21:48 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:21:48 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Iterate over the two-dimensional array
Whenever a redirection is found, check the type of
redirection and retrieve a file descriptor containing
the info we need as the infile
Check that the file descriptor that has been opened is valid (!= -1)
and continue
If a pipe is found, add a new node to the list of commands
In all other cases add whatever words are found
 to the argument list (argv) we call full_cmd
*/

/*
  Creates and fills a new array of strings from the `prompt` array.
  It starts filling from index `i` and fills `len` number of elements.
  Memory is dynamically allocated for the new array.

  Parameters:
      main_prompt: Pointer to the prompt structure
    - prompt: Pointer to the original array of strings.
    - i:      Starting index to begin filling from.
    - len:    Number of elements to fill in the new array.

  Returns:
    - Pointer to the newly created array of strings.
*/
char	**fill_arr(t_prompt *main_prompt, char **prompt, int i, int len) // başlangıç indexi ve uzunluğu gönderilmiş komut parçalarını beraber çalışacakları tespit edildiği için bir düğüm içine sırasıyla yerleştiren fonksiyon
{
	char	**temp; // komut parçalarının birleşmiş halinde basamak olarak kullanılacak ve döndürülecek olan değişken
	int		j; // baştan sona ilerlerken kullanılacak index

	j = 0; // başlangıçta 0
	temp = NULL; // başlangıçta NULL
	temp = get_grbg(main_prompt, len + 1, sizeof(char *)); // temp e gerekli olacak yer bellekte ayrılır
	if (!temp) // eğer yer ayrılırken bir hata oluştuysa
		return (NULL); // NULL döndürüp çık
	while (len > 0) // birleştirilmek için gönderilen komutların sayısı kadar komut dizisi dolaşılır
	{
		temp[j] = grbg_strdup(main_prompt, prompt[i]); // basamak olarak kullanılacak değişkenin ilk indexine gönderilen komutun başlangıç indexindeki komut kopyalanır
		i++; // gönderilen komut zincirindeki indexte bir ilerlenir
		j++; // parçalanmış halini tutacak olan değişkenin ilk indexinde de bir ilerlenir
		len--; // bölünüp kopyalanacak kısımın uzunluğu kadar bu döngü devam eder
	}
	temp[j] = NULL; // parçasını kopyaladığımız değişkenin sonuna NULL koyulur
	return (temp); // ve bu beraber işlenecek olan komut dizisi döndürülür
}

/*
Checks if the last character of
the command in the prompt is either '<', '>', or '|'.
If it's the case, prompt->stop = 1 and throw syntax error.
Parameters:
    - prompt: Pointer to the prompt structure containing input commands.

Returns:
- No return value.
*/
static void	check_last_char(t_prompt *prompt) // komut zincirinin son komut parçasını kontrol eder eğer sonrasında bir argüman olmayınca anlamsız olan bir komut sona yazıldıysa hatalı input tespiti yapar
{
	int		len_ar; // dizi uzunluğunu tutacak değişken
	char	last_char; // son karakteri tutacak değişken

	if (check_double_pipes(prompt) == 1) // if içindeki fonksiyon eğer komut içinde herhangibir yerde || varsa true döndürür
	{
		prompt->stop = 1; // stop flagi 1 yapılır komutlar işlenmeye devam etmesin diye
		return ; // fonksiyondan çıkılır çünkü bu projede || handle edilmek zorunda değil
	}
	len_ar = get_len_arr(prompt->commands) - 1; // komutun kaç stringden oluştuğunu tutar
	if (len_ar < 0 || prompt->commands[len_ar] == NULL
		|| prompt->commands[len_ar][0] == '\0') // komut zinciri uzunluğı hesaplanırken bir hata olduysa ya da sayılabilen son komut boşsa gibi durumlarda
		return ; // fonksiyondan çık bu da bir hatalı durumdur
	last_char = prompt->commands[len_ar][0]; // son komutun ilk karakterini değişkene attık
	if (last_char == '|' || last_char == '<' || last_char == '>' ) // eğer bu son komutun ilk karakteri |, < ya da > olursa 
	{
		prompt->stop = 1; // işlemleri durdurma flagini true yap
		syntax_error(prompt, prompt->commands[len_ar]); // syntax yani söz dizimi error larını yazdırdığımız fonksiyona sorunlu olan komut stringini de göndererek çağır
	}
}

/*
Checks if the last character of EACH command in the prompt is either '<', '>'.
If it's the case, prompt->stop = 1 and throw syntax error
Parameters:
    - prompt: Pointer to the prompt structure containing input commands.

Returns:
- No return value.
*/
static void	check_token(t_prompt *prompt) // tokenleri yani bölünmüş komut parçalarını kontrol eder hatalı bir yazım ya da sıralama var mı diye
{
	t_node		*current_node; // o an kontrol edilen düğümü tutan değişken sırasıyla bütün düğümleri tutacak kontrolleri bitene kadar
	t_cmddat	*cmd_data; // kontrol edilen düğümün içeriği yani düğümün data değişkeninin içindeki string
	int			len_arr; // komutun tamamının uzunluğunu tutan değişken
	char		last_char; // komut zincirindeki son komutu içindeki ilk karakteri tutacak

	current_node = prompt->cmd_list; // komut linked listesindeki ilk komut düğümünü current_node içine koyarak başlıyoruz
	while (current_node != NULL) // sıra sıra bütün komut düğümlerini dolaş sona gelene kadar
	{
		cmd_data = current_node->data; // mevcut komutun içeriğini cmd_data içine kopyalıyoruz
		len_arr = get_len_arr(cmd_data->full_cmd) - 1 ; // komutun tamamını kelimelere parçalanmış şekilde tutan değişkeni dizinin boyutunu döndüren fonksiyona gönderiyoruz
		last_char = cmd_data->full_cmd[len_arr][0]; // parçalanmış komutun son indexindeki kod parçasının ilk karakterini atıyoruz
		if (last_char == '|' || last_char == '<' || last_char == '>' ) // eğer komut zinciri sonundda |,<,> gibi komutlar varsa
		{
			prompt->stop = 1; // process lere daha fazla devam etmemek ve çalışan her şeyi durdurmak için stop flag ini true yapıyoruz
			syntax_error(prompt, cmd_data->full_cmd[len_arr]); // komut sonunda anlamsız klacak olan bu komutlardan birisi komut sonuna koyulduğu için söz dizimi hatasını ekranda gösterecek fonksiyonu çağırıyoruz
			free_node_list(prompt->cmd_list); // birbirine bağlı olan komut listesinin free leme işlemini yapıyoruz
			prompt->cmd_list = NULL; // komut listesini zaten free lemiştik şimdi de NULL koyup boşaltıyoruz
			break ; // döngüden çık
		}
		current_node = current_node->next; // bir sonraki komut düğümüne ilerle
	}
}

/*might lead to double frees with gc,
but seems to work @DENIZ DOUBLE FREE if error
@deniz is this comment still accurate (22/04)*/
static void	parse_remaining(t_prompt *prompt) // token kontrolü, redirection, env güncelleme ve tırnak temizleme gibi parserin devamı olan işlemleri yapan fonksiyon
{
	int	type; // redirection tipini tutacak değişken

	type = 0; // başlangıçta redirection yokmuş gibi 0
	check_token(prompt); // tokenlerde hatalı bir yazılım sıralama vs var mı diye kontrol eder varsa free lemeleri yapıp çıkar
	if (prompt->stop == 0) // eğer stop flagi hala false ise
		handle_redir(prompt, type); // redirection komutlarını (<, <<, >>, >) kontrol eder
	if (prompt->cmd_list != NULL) // eğer komut listesi boş değilse
		add_last_cmd_to_envp(prompt); // son komutun path veya env bilgilerini günceller
	get_rid_quotes(prompt); // zaten komut satırı bölündüğü için artık tırnak işaretleri string içlerinden silinir mesela echo "hello world"->["echo", ""hello world""] yerine ["echo", "hello world"] kalacak komut dizisi içinde parçalanmış halleri
}

void	parser(t_prompt *prompt, int i, int j) // lexer ile spacelerle bölünüp birleştirilen komut dizisini anlamlı parçalar halinde bölen fonksiyon mesela pipe bulursa pipe öncesi olan komutu ayrı pipe sonrası olan komutu ayrı çalıştırmak gerekir bu tarz durumlarda işi kolaylaştırmaya yarar
{
	t_cmddat	*ptr; // bölünecek komutu geçici olacak tutacak t_cmddat tipi pointer değişken

	ptr = NULL; // başlangıç değeri NULL
	check_last_char(prompt); // komut dizisinin son karakterini kontrol eder eğer |, <, > gibi komut sonunda anlamsız olan karakterler bulursa komuta devam etmez

	while (prompt && prompt->commands && prompt->commands[i] != NULL
		&& prompt->stop == 0) // eğer stop flagi herhangi bir yerde true yapılmadıysa ve komut dizisinin sonunda değilse
	{
		ptr = init_struct_cmd(prompt); // komut yapısının başlangıç değerlerininin verildiği fonksiyon çağıırılır
		if (!ptr) // eğer yer ayırırken ya da herhangi bir değer atama işleminde sorun olduysa
			return ; // fonksiyondan çık
		add_node_to_list(prompt, &(prompt->cmd_list), ptr); // komut listesinin sonuna ptr ile tuttuğumuz komut dizisini ekle
		while (prompt->commands[i] != NULL && prompt->commands[i][0] != '|') // komut listesinin sonunda değilsen ve elindeki komut | değilse
		{
			i++; // bulunduğumuz listede birleştirilecek olan komutların sonundaki indexi tutar
			j++; // bulunduğumuz listede birleştirilecek olan komutların uzunluğunu tutar
		}
		ptr->full_cmd = fill_arr(prompt, prompt->commands, i - j, j); // i-j başlangıç indexinden sonraki j tane stringi komutların tutulduğu linked list e ekler
		ptr->full_path = get_path_cmds(ptr, prompt->envp); // komutun çalıştırılabilir dosya yolunu full_path değişkenine aktaracak fonksiyon
		collect_grbg(prompt, ptr->full_path); // dosya yolunu tutacak değişkene de hafızada yer ayrılır
		if (prompt->commands[i] == NULL) // içinde gezdiğimiz bütün komutları tutan dizinin sonuna geldiysek
			break ; // döngüden çık
		i++; // listedeki bir sonraki komuta geç
		j = 0; // beraber çalışacak olan komutların hesaplanan uzunluğunu sıfırla
	}
	parse_remaining(prompt); // parser işlemlerinin devamını yapacak fonksiyon token hata kontrolleri, redirection handle, tırnakları silme, env güncelleme
}
