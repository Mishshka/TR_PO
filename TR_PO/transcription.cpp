//transcription.cpp
/*
Создатель: Покачалов М.Р.
-------------------------------------------------------------------------------------------
Данный код описывает модуль (блок) связанный с обработкой слов и выдачей их транскрипций и ударения
Порядок работы программы:
	1. Получаем вектор пар слово + номер предложения
	2. Переводим в нужную для работы программы кодировку (в utf-8)
	3. Записываем во временный файл полученные словаъ
	4. Передаем программе файл и получаем выходной файл с транскрипциями
	5. Считываем и записываем в вектор транскрипции
	6. Возвращаем вектор
--------------------------------------------------------------------------------------------*/

//подключаемые файлы:
#include "transcription.h"
//---------------------------------

// функция перевода из 1251 в utf8 
//возвращает слово в соответствующей кодировке
string cp1251_to_utf8(const string& str1251)
{
	if (str1251.empty()) return "";

	//CP1251 -> UTF-16 |wchar|
	int wsize = MultiByteToWideChar(1251, 0, str1251.c_str(), -1, 0, 0);
	wstring wstr(wsize, 0);
	MultiByteToWideChar(1251, 0, str1251.c_str(), -1, &wstr[0], wsize);

	//UTF-16 -> UTF-8
	int u8size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, 0, 0, 0, 0);
	string utf8str(u8size, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8str[0], u8size, 0, 0);

	if (!utf8str.empty() && utf8str.back() == '\0')
		utf8str.pop_back();

	return utf8str;
}

// функция получения транскрипции (!!!!!в UTF-8)
//возвращает вектор транскрипций
vector <string> get_transcript(vector<pair<string, int>> words)
{
	//работаем по следующему принципу:
	//	1. Проверяем, что вектор не пустой
	//	2. Делаем временные файлы, в которые у нас запишутся входная (слово + номер) и выходная (транскрипция) информация
	// (логично что сначала заполним входной, а выходной заполнится только после работы программы)
	//	3. Запускаем программу и передаем ей имена входного и выходного файла
	//	4. Считываем из выходного файла все и возвращаем
	// ценок
	// не забываем удалить временные файлы
	//

	vector <string> transcriptions;

	if (words.empty()) return transcriptions;

	const string input = "tmp_words.txt";
	const string output = "tmp_transcript";

	ofstream outFile(input);
	for (const auto& word : words)
		outFile << cp1251_to_utf8(word.first) << endl;
	outFile.close();

	string exePath = "eSpeak NG\\espeak-ng.exe";
	string com = "\"\"" + exePath + "\" -v ru --ipa --path=\"eSpeak NG\" -f " + input + " -q > " + output + "\"";
	system(com.c_str());

	ifstream inFile(output);
	string token;
	while (inFile >> token)
	{
		transcriptions.push_back(token);
	}
	inFile.close();

	remove(input.c_str());
	remove(output.c_str());

	return transcriptions;
}