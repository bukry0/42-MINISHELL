/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:28:09 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:28:09 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
diğer error fonksiyonundan farkı anlamlı bazı komut parçaları girilmiş yani diğer error fonksiyonlarına girmiyor
ama birleştirildiğinde birbirleriyle mantıklı bir komut zinciri oluşturmuyorlar
örneğin cd /geçersiz/bir/dizin normalde kod bunu parçaladığında ilk aşamada komutlar doğru görünür
ama buna dayanıp bir prompt başlatırsak olmayan bir dizin için döngü içerisinde uğraşıyor olacağız
ama biz prompt->stop = 1 olarak ayarlayarak bu komut zincirinin işlemlerini tamamlamak için daha fazla uğraşmamasını
döngülerden çıkıp yeni bir satır başlatmasını söylüyoruz
*/ 

int	syntax_error(t_prompt *prompt, char *token) // söz dizimi hatalarını handle edip yeni komut satırına geçiren fonksiyon
{
	if (prompt) // prompt değişkeni hatalı/boş gönderilmediyse
		prompt->stop = 1; // stop flag değişkenini 1 yapıyoruz bu programa daha fazla devam edilemeyecek bir hatanın girildiği anlamına gelir ve bu flagle bu komut döngüsünü bitirmeden çıkıp yeni komut satırına geçilir
	if (!token) // eğer token yoksa (pipe, | ile bitmiş bir satır ya da kapatılmamış bir tırnak çifti gibi)
		print_err_msg(NULL, "syntax error near unexpected token 'newline'"); // syntax için hata olduğunu yazdıracak error fonksiyonu çağrılır
	else
	{
		ft_putstr_fd("minishell: syntax error near unexpected token '", 2); // hangi token sebepli error verildiğini yazdıran satır
		ft_putstr_fd(token, 2); // token eklenir mesela |, ", << vb.
		ft_putstr_fd("'\n", 2);
	}
	return (0); // fonksiyon başarılı biterse 0 döndürülür
}

/*@deniz you commented this, I put it here for norm :
//replace with below print_err_msg_lng if possible */
void	print_err_msg(char *cmd, char *msg) // sadece komut varsa yazılacak hata mesajının düzenlendiği fonksiyon
{
	if (!msg) // eğer hata için spesifik bir mesaj girilmediyse
		return ; // fonksiyondan çık
	ft_putstr_fd("minishell: ", 2); // 2 error kodu kullanılarak hata yazılır
	if (cmd) // hata kaynağı olan satıra girilen komut
	{
		ft_putstr_fd(cmd, 2); // komut 2 error koduyla yazılır
		ft_putstr_fd(": ", 2);
	}
	ft_putstr_fd(msg, 2); // belirlenen hata mesajı da 2 error koduyla yazıdırlır
	ft_putstr_fd("\n", 2);
}

// Sılacım bu iki fonksiyonun işlevinin aynı olduğunu düşünüyorum
// o yüzden de alttaki fonksiyonu sadece kullanmamız yeterli diye muhtemelen diğerini sileceğiz
// arg yerine NULL gönderecek şekilde kullanıldıkları bütün yerleri düzenleriz en son haber vereyim dedim <3
// bir de azıcık vicdani de bir tarafı var tabi minik de olsa kendimiz de dokunuşlar yaparsak vicdanımız rahatlar belki hahahsjaj

/*@deniz you commented this, I put it here for norm :
 different -> compare all err msgs with bash
*/
int	print_err_msg_lng(char *cmd, char *msg, char *arg) // hem komut hem argüman bulunan hata mesajlarını yazdırmak için kullanılan fonksiyon
{
	ft_putstr_fd("minishell: ", 2); // 2 error koduyla hata yazılmaya başlar
	if (cmd) // komut boş değilse
		ft_putstr_fd(cmd, 2); // hatalı girilen komut 2 error koduyla yazılır
	if (arg) // argüman boş değilse
	{
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(arg, 2); // hatalı girilen satırdaki argüman 2 error koduyla yazılır
	}
	if (msg) // hata için belirlenen spesifik mesaj varsa
	{
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(msg, 2); // mesaj da 2 error koduyla yazılır
	}
	ft_putstr_fd("\n", 2);
	return (1); // mesaj doğru bir şekilde iletildiyse fonksiyon 1 döner
}

char	*add_to_str(t_prompt *prompt, char **str, char *add) // verilen string sonuna add değişkeni içindeki stringi de ekleyen fonksiyon
{
	char	*new; // istenilen stringin de eklenmiş haliyle döndürülecek stringin son halini tutacak değişken

	if (!add) // eğer eklenmesi planlanan string boş ise
	{
		new = grbg_strdup(prompt, *str); // yeni stringe yer ayrılır ve eski string içine kopyalanır
		return (new); // eski stringin aynısı yeni bir allocation daha yapılarak döndürülür
	}
	if (!str || !*str) // eğer üzerine eklenecek string başta boş ise ya da adresi boş ise
	{
		new = grbg_strdup(prompt, add); // üzerine eklenebilecek bir string olmadığı için sadece add değişkenindeki string new değişkenine aktarılır
		return (new); // içerisinde sadece add değişkenindeki metin bulunan new değişkeninin allocation yapılmış hali döndürülür
	}
	new = grbg_strjoin(prompt, *str, add); // iki string de dolu ise join ile str sonun add stringi de eklenir
	return (new); // stringlerin birleşmiş ve allocation yapılmış hali döndürülür
}
