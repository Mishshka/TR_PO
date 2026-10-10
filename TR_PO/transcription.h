#pragma once
//transcription.h
//Создатель: Покачалов М.Р.

//Используемые библиотеки:
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <windows.h>
//--------------------------------

//Используемые пространства имен:
using namespace std;
//________________________________

//----------------------------------------------------------------------

//Используемые функции:


//функция перевода из cp1251 в utf-8
string cp1251_to_utf8(const string& str1251);

//функция получения транскрипции
vector <string> get_transcript(vector<pair<string, int>> words);