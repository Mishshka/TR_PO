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
vector <pair<string, int>> define_noun(const vector<pair<string, int>>& sentences)
{
	//выходной вектор
	vector<pair<string, int>> out;

	//записываем полученное предложение в временный файл (для mystem)
	ofstream f("tmp_in.txt");
	for (const auto& p : sentences) 
	{
		/*f.write(p.first.c_str(), p.first.size());
		f.put('\n');*/
		f << "sentence\n";
		f << p.first << "\n\n";

	}
	f.close();

	//запускаем mystem: вход — файл, выход — файл
	// ключ -e cp1251 указывает mystem кодировку входа
	int rc = system("mystem -i -d -e cp1251 tmp_in.txt > tmp_out.txt");
	if (rc != 0)
	{
		remove("tmp_in.txt");
		remove("tmp_out.txt");
		return out;
	}

	//читаем результат
	ifstream in("tmp_out.txt");

	string output((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

	istringstream stream(output);
	string line_out;
	int sentence_index = -1;

	while (getline(stream,line_out))
	{
		if (!line_out.empty() && line_out.back() == '\r') {
			line_out.pop_back();
		}

		//ищем поблочно (через поиск блоков фигурных скобок)
		size_t pos = 0;
		while (pos < line_out.size())
		{
			size_t next_sent = line_out.find("sentence", pos);
			size_t next_block = line_out.find('{', pos);

			// "sentence" встретилось раньше блока (или блока больше нет)
			if (next_sent != string::npos &&
				(next_block == string::npos || next_sent < next_block))
			{
				sentence_index+=1;                 // +1 за каждое вхождение
				pos = next_sent + 8;              // длина "sentence"
				continue;
			}

			if (next_block == string::npos) break;

			size_t end_pos = line_out.find('}', next_block);
			if (end_pos == string::npos) break;

			string block = line_out.substr(next_block + 1, end_pos - next_block - 1);

			if (block.find("=S,") != string::npos)
			{
				size_t word_start = line_out.rfind('}', next_block);
				word_start = (word_start == string::npos) ? 0 : word_start + 1;
				string noun = line_out.substr(word_start, next_block - word_start);

				if (sentence_index >= 0 && sentence_index < (int)sentences.size())
				{
					int num_sentence = sentences[sentence_index].second;
					out.push_back(make_pair(noun, num_sentence));
				}
			}

			pos = end_pos + 1;
		}
	}

	in.close();

	//убираем временные файлы
	remove("tmp_in.txt");
	remove("tmp_out.txt");

	return out;
}