//define_noun.cpp
/*
Создатель: Покачалов М.Р.
-------------------------------------------------------------------------------------------
Данный код описывает модуль (блок) связанный с обработкой предложений и определением частей речи слов, и дальнейше передачей
модулю по работе с транскрипциями
Порядок работы программы:
	1. При получении сигнала (вызов функции при чтении файла) о необходимости работать, получаем предложение и номер
	2. Запускаем Mystem и передаем ему предложение
	3. Получаем разбор слов и выбираем те, что являются существительными
	4. Отправляем в модуль работы с транскрипциями отобранные слова (по одному) + номер предложения
--------------------------------------------------------------------------------------------*/

//подключаемые файлы:
#include "define_noun.h"
//---------------------------------


//функция начала обработки
//получает предложение и его номер
vector <pair<string, int>> define_noun(string sentence, int num_sentence)
{
	string noun;
	//записываем полученное предложение в временный файл (для mystem)
	ofstream f("tmp_in.txt");
	f.write(sentence.c_str(), sentence.size());
	f.put('\n');
	f.close();

	//выходной вектор
	vector <pair<string, int>> out;

	//запускаем mystem: вход — файл, выход — файл
	// ключ -e cp1251 указывает mystem кодировку входа
	int rc = system("mystem -i -d -e cp1251 tmp_in.txt > tmp_out.txt");
	
	
	//читаем результат
	ifstream in("tmp_out.txt");
	string output((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

	//ищем поблочно (через поиск блоков фигурных скобок)
	size_t pos = 0;
	while ((pos = output.find('{', pos)) != string::npos)
	{
		size_t end_pos = output.find('}', pos);
		
		if (end_pos == string::npos)
			break;
		string block = output.substr(pos + 1, end_pos - pos - 1);
		
		if (block.find("=S,") != string::npos)
		{
			size_t word_start = output.rfind('}', pos);
			word_start = (word_start == string::npos) ? 0 : word_start + 1;
			string noun = output.substr(word_start, pos - word_start);
			//после передаем в выходной вектор
			out.push_back(make_pair(noun, num_sentence));
		}
		// Сдвигаем позицию для поиска следующей пары скобок
		pos = end_pos + 1;
	}
	
	//убираем временные файлы
	in.close();
	remove("tmp_in.txt");
	remove("tmp_out.txt");

	return out;
}