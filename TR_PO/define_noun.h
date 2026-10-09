#pragma once
//define_noun.h
//Создатель: Покачалов М.Р.

//Используемые библиотеки:
#include <iostream>
#include <string>
#include <Windows.h>
#include <cstdio>
#include <conio.h>
#include <fstream>
#include <vector>
//--------------------------------

//Используемые пространства имен:
using namespace std;
//________________________________
	//Константные переменные, используемые для вывода "технического" текста:

	//Сообщение о неоткрывшемся файле 

//----------------------------------------------------------------------

//Используемые функции:

//функция для начала работы и определения части речи 
vector <pair<string, int>> define_noun(string sentence, int num_sentence);