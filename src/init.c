/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:16:52 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:16:52 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Initializes the prompt structure `prompt` with the
provided environment variables `envp`.

Parameters:
- prompt: Pointer to the prompt structure to initialize.
- envp:   Array of strings representing the environment variables.

Note: This function also initializes the environment
within the prompt structure using the `init_env` function.

Returns: None
*/

void	init_prompt_struct(t_prompt *prompt, char **envp) // prompt isimli structın içeriğini verilen envp ortam değişkenleriyle başlatır yani şuanda bulunulan dizin bilgileri vs
{
	prompt->input_string = NULL; // kullanıcının ekrana gireceği komutu tutacak değişken, başlangıçta NULL
	prompt->commands = NULL; // girilen komutların anlamlı parçalanmış hallerini (tokenize) tutuyor, başlangıçta NULL
	prompt->cmd_list = NULL; // ayrılmış komutların linked list halinde tutulduğu değişken, başlangıçta NULL
	prompt->pid = -1; // çalıştırılacak child process in PROCESS ID si, henüz fork edilmediği için başlangıç değeri -1
	prompt->stop = 0; // çıkış için kullanılan CTRL+D komutunun flagini tutar, başlangıçta ve çıkış işlemi gelmediği sürece değeri 0
	prompt->grbg_lst = NULL; // allocation yapılan bütün değişkenlerin tutulduğu linked list, henüz allocation yapılmadığı için başlangıçta NULL
	init_env(prompt, envp); // envp değişkeni içine sistemden atılan başlangıç ortam değişkenlerini kullanılabilir bir hale getirecek olan fonksiyon çağırılır
}

/*
Initializes the environment for the minishell by copying the provided
environment variables `env` into the `prompt->envp` array.

Parameters:
- prompt: Pointer to the prompt structure containing environment information.
- env:    Array of strings representing the environment variables.

Returns:
  - 1 if the initialization is successful.
  - 0 if an error occurs during memory allocation or copying.
*/

int	init_env(t_prompt *prompt, char **env) // program başlatılırken sistemden alınan env değişkenini shell komutlarında kullanabileceğimiz bir versiyona çevirip shell boyunca kullanacağımız prompt->envp değişkenine atar
{
	int	i; // ortam değişkenlerini prompt a kopyalarken kullanacağımız döngüdeki sayaç

	i = 0;
	prompt->envp = get_grbg(prompt, get_len_arr(env) + 1, sizeof(char *)); // prompt->envp için allocation yapılıyor aynı zamanda da garbage sistemiyle program sonunda free lenecek olan değişkenler listesine ekleyen fonksiyona gönderiliyor
	if (!prompt->envp) // bellek ayırmada sorun olursa girilecek şart
	{
		printf("Error! About Environment Allocation.\n"); // hatanın mesajı
		return (0); // başarısız olduğunda 0 döner fonksiyon
	}
	while (env[i]) // env dizisi boyunca döner ve sistemden alınan değerleri tek tek prompt->envp değişkenine aktarır
	{
		prompt->envp[i] = grbg_strdup(prompt, env[i]); // grbg_strdup ile aktarma işlemi birer birer yapılır
		if (!prompt->envp[i]) // aktarmada bir allocation hatası olursa girer
		{
			printf("Error! About Environment Allocation.\n"); // error ekrana yazılır
			return (0); // fonksiyon başarısız 0 döner
		}
		i++;
	}
	prompt->envp[i] = NULL; // hepsi aktarıldı son dizeye NULL koyduk kapanış için
	return (1); // program başarılı
}

/*
  Initializes a new structure t_cmddat with default values.
  Allocates memory for the structure and returns a pointer to it.

  Returns:
    - Pointer to the newly initialized t_cmddat structure.
*/

t_cmddat	*init_struct_cmd(t_prompt *prompt) // komut yapısını tutan değişkenlerin başlangıç değerlerini veren fonksiyon
{
	t_cmddat	*ptr; // komutları tutması için geçici bir pointer değişkeni

	ptr = get_grbg(prompt, 1, sizeof(t_cmddat)); // t_cmddat boyutunda alan açılır garbage mantığı ile
	if (!ptr) // eğer yer ayrılırken bir sorun ile karşılaşıldıysa
		return (NULL); // fonksiyondan çık
	ptr->full_cmd = NULL; // komutun tamamını tutan değişkene başlangıç değeri olan NULL koyulur
	ptr->full_path = NULL; // komutun çalıştırılabilir dosya yolunu tutacak değişken başlangıç değeri NULL sonrasında get_path_cmds fonksiyonuyla doldurulacak ls gibi komutlar çağırılırsa kullanılacak
	ptr->infile = STDIN_FILENO; // komutun standart inputu yani 0 olarak ayarlanır eğer < gibi komutlar olursa o zaman verilen dosyanın fd değeri ile değiştirilecek
	ptr->outfile = STDOUT_FILENO; // komutun standart outputu olarak 1 atanır eğer output bir dosyaya yönlendirilirse >> o zaman o dosyanın fd si ile değiştirilir
	ptr->prompt = prompt; // Bu komutun ait olduğu shell context'ini (t_prompt) gösterir yani environment değişkenlerine, garbage collectora, stop flagine erişim için gerekli
	ptr->file_open = 0; // eğer bir dosyaya redirection yapıldıysa >, < vs gibi bu flag 1 yapılır ki sonrasında exec aşamaları vs bittiğinde kapatılması gereken bir dosya var mı diye kontrol etmesi kolaylaşsın
	return (ptr); // komutların başlangıç değerleri ayarlanmış olan pointer döndürülür
}
