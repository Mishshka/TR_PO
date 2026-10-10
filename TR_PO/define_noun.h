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
#include <sstream>
//--------------------------------

//Используемые пространства имен:
using namespace std;
//________________________________

//----------------------------------------------------------------------

//Используемые функции:

//функция для начала работы и определения части речи 
vector <pair<string, int>> define_noun(const vector<pair<string, int>>& sentences);