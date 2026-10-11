//read_file.cpp
/*
Создатель: Покачалов М.Р.
-------------------------------------------------------------------------------------------
Данный код описывает модуль (блок) связанный с чтением файла (посимвольно) и передачей слов модулю (блоку) обработки частей речи
Порядок работы программы:
	1. При получении сигнала (вызов функции из консоли) о необходимости работать, получаем имя файла
	2. Открываем файл и начинаем работу с ним:
		2.1. Считываем посимвольно до пробела / знака пунктуации и формируем слово
		2.2. Сформированное слово проверяем на отсутствие "инородных" символов (должны быть только кириллические буквы и возможен дефис)
		2.3.В случае успеха добавляем слово в текущее предложение
		2.4. При достижении конца предложения - отправляем накопленное предложение в модуль обработки частей речи
		2.5 После обработки получаем вектор из пар слово - номер предложения
		2.4. Если при чтении обнаруживаем знак пунктуации - увеличиваем на еденицу счетчик номера предложений
		2.5. После того, как прочитали конец файла - завершаем работу и отправляем накопленный выходной вектор в модуль транскрипции и оттуда в дальнейшую обработку
--------------------------------------------------------------------------------------------*/

//подключаемые файлы:
#include "read_file.h"
//---------------------------------

//функция определения являетс ли символ разделителем
//возвращает истина если символ таковым является
bool is_word_separator(char c)
{
	if (c == ' ' || c == '\t' || c == '\n' || c == '.' || c == '!' || c == ',' ||
		c == ':' || c == ';' || c == '—' || c == '?' || c == '(' ||
		c == ')' || c == '[' || c == ']' || c == '\'' || c == '\"' || c == '<' || c == '>'
		|| c == '+' || c == '=' || c == '_' || c == '{' || c == '}' || c == '%' || c == '@'
		|| c == '#' || c == '№' || c == '$' || c == '^' || c == '&' || c == '*' || c == '~' || c == '`')
		return true;
	
	return false;
}

//функция определения являются ли все символы кириллическими
bool is_cyrillic(string word)
{
	//допускаем использование дефиса, но только не первым  или последним символом 
	if (word[0] == '-' || word[word.length() - 1] == '-')
		return false;

	for (char c : word)
	{
		
		if (c == '-')
			continue;
		
		//Проверяем входит ли символ в допустимый диапазон
		if (!(c >= 'А' && c <= 'Я' || c >= 'а' && c <= 'я' || c == 'Ё' || c == 'ё'))
			return false;
	}

	return true;
}

//функция определения конца предложения 
bool is_sent_end(char c)
{
	return (c == '.' || c == '!' || c == ';' || c == '?');
}

//функция начала работы
//получает имя выбранного файла
void read_file(string file_name)
{
	auto start = std::chrono::system_clock::now();

	//установка кодировки
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	ifstream file(file_name);

	if (!file.is_open())
	{
		cout << file_err_mes;
		int ch = _getch();
		return;
	}

	//переменная сформированного слова
	string word;
	//переменная сформированного предложения
	string sentence;
	//счетчик текущего номера предложения
	int num_sentence = 1;
	//считываемый символ
	char c;

	vector <pair<string, int>> in;
	//vector <pair<string, int>> noun_sentence;

	while (file.get(c))
	{
		if (is_word_separator(c))
		{
			//если есть слово - проверяем его
			if (!word.empty())
			{
				if (is_cyrillic(word))
				{
					//добавления слова в предложение
					sentence += word;
					sentence += c;
				}
			}
			word.clear();

			//если предложение кончилось увеличиваем на 1 счетчик
			if (is_sent_end(c))
			{
				//проверяем не идет ли сразу несколько знаков препинания
				while (file.peek() != EOF && is_sent_end(file.peek()))
					file.get();
				if (!sentence.empty())
				{
					in.push_back(make_pair(sentence, num_sentence));
					//vector <pair<string, int>> in = define_noun(sentence, num_sentence);
					//noun_sentence.insert(noun_sentence.end(), in.begin(), in.end());
					
					sentence.clear();
				}
				num_sentence++;

			}
		}

		else
			word += c;
	}

	//в случае если файл не закончился разделителем проверяем последнее слово
	//если есть слово - проверяем его
	if (!word.empty())
	{
		if (is_cyrillic(word))
		{
			//добавления слова в предложение
			sentence += word;
		}
	}
	
	word.clear();

	if (!sentence.empty())
	{
		//получение существительных из предложения
		in.push_back(make_pair(sentence, num_sentence));
		//vector <pair<string, int>> in = define_noun(sentence, num_sentence);
		//noun_sentence.insert(noun_sentence.end(), in.begin(), in.end());

		sentence.clear();
	}

	file.close();
	
	

	//выходной вектор из пар слово - номер предложения 
	vector <pair<string, int>> noun_sentence = define_noun(in);
	
	//получаем транскрипции
	vector<string> transcriptions = get_transcript(noun_sentence);

	//формируем выходной файл
	find_rithms(noun_sentence, transcriptions, file_name);
	//find_rithms(noun_sentence, file_name);

	auto end = std::chrono::system_clock::now();
	cout <<endl<< chrono::duration_cast<chrono::milliseconds>(end - start) << endl;
	_getch();
}