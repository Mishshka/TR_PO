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
//vector <pair<string, int>> define_noun(string& sentence, int& num_sentence)
{
	//выходной вектор
	//string noun;
	vector<pair<string, int>> out;

	//записываем полученное предложение в временный файл (для mystem)
	ofstream f("tmp_in.txt");
	/*f.write(sentence.c_str(), sentence.size());
	f.put('\n');*/
	
	for (const auto& p : sentences) 
	{
		f.write(p.first.c_str(), p.first.size());
		f.put('\n');
	}
	f.close();

	//запускаем mystem: вход — файл, выход — файл
	// ключ -e cp1251 указывает mystem кодировку входа
	int rc = system("mystem -i -d -e cp1251 tmp_in.txt > tmp_out.txt");
	if (rc != 0) return out;
	
	//читаем результат
	ifstream in("tmp_out.txt");
	string output((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	in.close();

	istringstream stream(output);
	string line_out;
	size_t sentence_index = 0;

	while (getline(stream,line_out) && sentence_index <sentences.size())
	{
		if (!line_out.empty() && line_out.back() == '\r') {
			line_out.pop_back();
		}
	
		int num_sentence = sentences[sentence_index].second;

		//ищем поблочно (через поиск блоков фигурных скобок)
		size_t pos = 0;
		//while ((pos = output.find('{', pos)) != string::npos)
		while ((pos = line_out.find('{', pos)) != string::npos)
		{
			/*size_t end_pos = output.find('}', pos);

			if (end_pos == string::npos)
				break;
			string block = output.substr(pos + 1, end_pos - pos - 1);*/

			size_t end_pos = line_out.find('}', pos);

			if (end_pos == string::npos) break;

			string block = line_out.substr(pos + 1, end_pos - pos - 1);

			if (block.find("=S,") != string::npos)
			{
				//size_t word_start = output.rfind('}', pos);
				//word_start = (word_start == string::npos) ? 0 : word_start + 1;
				//string noun = output.substr(word_start, pos - word_start);
				////после передаем в выходной вектор
				//out.push_back(make_pair(noun, num_sentence));

				size_t word_start = line_out.rfind('}', pos);
				word_start = (word_start == string::npos) ? 0 : word_start + 1;
				string noun = line_out.substr(word_start, pos - word_start);
				//после передаем в выходной вектор
				out.push_back(make_pair(noun, num_sentence));
			}
			// Сдвигаем позицию для поиска следующей пары скобок
			pos = end_pos + 1;
		}
		sentence_index++;
	}

	//убираем временные файлы
	remove("tmp_in.txt");
	remove("tmp_out.txt");

	return out;
}