#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>

#include "transcription.h"
#include <Windows.h>
using namespace std;

struct Prop
{
    long long number;
    Prop* next_prop;
};

struct Noun     //информация о существительном
{
    string noun;        //существительное       
    Prop* prop_list;      //номер предложения
    long long repetitions;      //число повторений
    Noun* next_noun;      //указатель на следующее существительное в группе
};


struct Ending   //информация о концовке слова
{
    string ending;          //концовка
    Noun* first_in_column;  //первое существительное с такой концовкой
    Ending* next_ending;    //указатель на следующую концовка
    long long counter = 0;
};

string get_ending(const string& transcription);
//string get_ending(const string& noun);          //получение концовки слова

void put_in_struct(pair<string, long long> ex_of_noun, const string& transcription, Ending*& first_ending, bool& find_flag);
//void put_in_struct(pair<string, int> ex_of_noun, Ending*& first_ending, bool& find_flag);        //помещение существительного в структуру

void sort_nouns(const vector<pair<string, long long>>& nouns, const vector<string>& transcriptions, Ending*& first_ending, bool& find_flag);
//void sort_nouns(const vector<pair<string, int>>& nouns, Ending*& first_ending, bool& find_flag);

void delete_noun(Noun* noun);                //очистка списка слов

void delete_ending(Ending* ending);          //очистка списка концовок


void clean_struct(Ending*& first_ending);         //очистка структуры

long long factorial(long long n);

long long combins(long long n, long long k);

long long count_rithms(Ending* first_ending);

void write_to_file(string inp_name, Ending* first_ending);       //формирование выходного файла

void find_rithms(vector<pair<string, long long>>& nouns, vector<string>& transcriptions, string& inp_name);
//void find_rithms(vector<pair<string, int>> nouns, string inp_name);
