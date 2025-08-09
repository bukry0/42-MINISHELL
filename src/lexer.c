/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:17:37 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:17:37 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	is_only_space(const char *str) // gönderilen stringdeki karakterlerin tamamı space vb mi yoksa farklı herhangi bir karakter var mı bunun tespitini yapan fonksiyon
{
	while (*str != '\0') // gönderilen stringin son karakterine kadar tek tek ilerle
	{
		if (!isspace(*str)) // karakter space değilse 
			return (0); // 0 döndür
		str++;
	}
	return (1); // eğer gönderilen stringde ki bütün karakterler space ve benzeri karakterler ise 1 döndürür
}

void	lexer(t_prompt *prompt) // kullanıcının girdiği komut satırının alınıp analiz edildiği ve bölündüğü fonksiyon
{
	prompt->input_string = readline("minishell$ "); // kullanıcıdan minishell$ promptuyla bir komut ister
	collect_grbg(prompt, prompt->input_string); // input_string değişkenine bellekte yer açılmış olabilşir o yüzden garbage a ekliyoruz ki program sonunda topluca freeleme işlemi yapılırken gözden kaçıp leak sorunu çıkartmasın
	signals_non_interactive(); // sinyallerin input alındıktan sonraki davranışlarını aktif edecek hazır fonksiyonu çalıştırır, komut işlenip bitene kadar bu kural devam devam eder
	add_history(prompt->input_string); // sonraki komut satırı açıldığında yukarı yön tuşuna bastığımızda şuan girdiğimiz komuta erişebilmemiz için history e bu komutu ekleyecek fonksiyon
	if (prompt->input_string == NULL) // CTRL+D ile çıkıldıysa ya da terminal kapandıysa 
	{
		prompt->stop = 1; // stop flagini 1 yapıp
		exit_ms(0, prompt); // exit_ms fonksiyonuyla kontrollü bir şekilde programdan çıkma hazırlıkları yapılır
		return ; // programdan da çıkılmak üzere fonksiyondan çıkılır yani komut satırlarımız sonlanır
	}
	if (ft_strlen(prompt->input_string) <= 0
		|| is_only_space(prompt->input_string)) // komut satırına hiçbir karakter girilmeden ya da sadece space vb karakterler girilip enter yapıldıysa
	{
		prompt->stop = 1; // stop flagi 1 yapılıp
		return ; // fonksiyondan çıkılır yani sadece şuan bulunduğumuz satır sonlandırılıp yeni satıra geçilir
	}
	prompt->input_string = handle_spaces(prompt, prompt->input_string, 0, 0); // kullanıcının girdiği komut satırındakieksik olan space karakterlerini ekler parçalarken sistematik ilerleyebilmek için
	prompt->commands = split_input(prompt->input_string, prompt); // girilen komutu space karakterlerine ve tırnaklara uygun bir şekilde böler, normalde split sadece space lerden böler bu da normalde bölmemesi gerken tırnak içindeki metinleri de parçalar fakat bu fonksiyon shell komutlarına uygun bölmeler yapacak
	if (prompt->commands == NULL) // eğer komutlar doğru bir şekilde oluşturulamadıysa 
		prompt->stop = 1; // stop flagi 1 yapılır bunun sayesinde de program devamında işlem yapılmaz
	if (prompt->stop != 1) // eğer stop flagi 1 değilse yani fark edilen bir hatadan dolayı durdurma işlemi true olarak değiştirilmediyse
		prompt->commands = expander(prompt, prompt->commands, prompt->envp); // eğer komutları doğru bir şekilde bölüştürebildiysek prompt içerisindeki environment değişkenlerini olması gerektiği gibi dolduracak fonksiyon çağırılır
}
