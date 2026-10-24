#include "out_work.h"

string get_ending(const string& transcription)//получение концовки слова
  
{
    
    size_t stress_pos = transcription.find("\xCB\x88");
    if (stress_pos == string::npos) return transcription;
    string ending = transcription.substr(stress_pos);
    return ending;
}


void put_in_struct(pair<string, int> ex_of_noun, const string& transcription, Ending*& first_ending) //помещение существительного в структуру
{
    string noun = ex_of_noun.first;
    int prop_number = ex_of_noun.second;
    string ending = get_ending(transcription); //получение концовки
       

    Ending* curr_ending = first_ending;                 //выбор текущей концовки
    Ending* prev_ending = nullptr;                      //выбор предыдущей концовки

    if (first_ending != nullptr)                          //поиск места слова в структуре, если структура не пуста
    {
        while (curr_ending != nullptr)                  //цикл прохода по списку встреченных ранее концовок
        {
            if (ending != curr_ending->ending)          //концовка рассматриваемого слова не совпала с текущей->сдвиг текущей и предыдущей концовок
            {
                prev_ending = curr_ending;
                curr_ending = prev_ending->next_ending;
            }
            else
            {
                break;                                 //выход из цикла при нахождении в списке кронцовки слова
            }
        }
        if (curr_ending == nullptr)                     //концовка слова не встретилась в списке
        {
            curr_ending = new Ending;                   //создание нового элемента списка
            curr_ending->ending = ending;
            curr_ending->next_ending = nullptr;
            curr_ending->first_in_column = nullptr;
            prev_ending->next_ending = curr_ending;     //присоединение нового элемента к списку
        }
    }
    else                                                //если структура пуста
    {
        curr_ending = new Ending;
        curr_ending->ending = ending;
        curr_ending->next_ending = nullptr;
        curr_ending->first_in_column = nullptr;
        first_ending = curr_ending;                     //установка указателя первого элемента
    }
    Noun* curr_noun = nullptr;
    Noun* prev_noun = nullptr;
    if (curr_ending->first_in_column == nullptr)        //если до этого не встречались слова с такой концовкой
    {
        curr_noun = new Noun;
        curr_ending->first_in_column = curr_noun;       //установка указателя начала списка слов с такой концовкой на новое слово
    }
    else                                                //если до этого встречались слова с такой концовкой
    {
        curr_noun = curr_ending->first_in_column;

        while (curr_noun != nullptr)                     //проход по списку слов с такой же концовкой
        {
            if (curr_noun->noun != noun)
            {
                prev_noun = curr_noun;                    //продвижение по списку, если слово не совпало
                curr_noun = prev_noun->next_noun;
            }
            else break;                                    //выход из цикла, если слово совпало
        }
        if (curr_noun != nullptr)                           //если слово уже встречалось
        {
            curr_noun->repetitions++;                       //увеличиваем счётчик повторений
            Prop* curr_prop = curr_noun->prop_list;
            while (curr_prop->next_prop != nullptr)
                curr_prop = curr_prop->next_prop;
            Prop* add_prop = new Prop;
            add_prop->number = prop_number;
            add_prop->next_prop = nullptr;
            curr_prop->next_prop = add_prop;
            return;
        }
        curr_noun = new Noun;                           //создание нового элемента списка слов с такой концовкой
        if (prev_noun != nullptr)
        {
            prev_noun->next_noun = curr_noun;
        }
    }
    curr_ending->counter++;
    curr_noun->prop_list = new Prop;
    curr_noun->prop_list->number=prop_number;
    curr_noun->prop_list->next_prop = nullptr;
    curr_noun->repetitions = 1;
    curr_noun->noun = noun;
    curr_noun->next_noun = nullptr;
}

void sort_nouns(const vector<pair<string, int>>& nouns, const vector<string>& transcriptions, Ending*& first_ending)
//void sort_nouns(const vector<pair<string, int>>& nouns, Ending*& first_ending, bool& find_flag)
{
    for (int i = 0; i < nouns.size(); i++)
    {
        put_in_struct(nouns[i], transcriptions[i], first_ending);
        //put_in_struct(nouns[i], first_ending, find_flag);
    }
}

void clear_prop_list(Prop* prop)
{
    if (prop != nullptr)
    {
        clear_prop_list(prop->next_prop);
        delete prop;
    }
}


void delete_noun(Noun* noun)                //очистка списка слов
{
    if (noun != nullptr)
    {
        if (noun->next_noun != nullptr)     //пока не дошли до конца списка
        {
            delete_noun(noun->next_noun);   //рекурсивный вызов
        } 
        clear_prop_list(noun->prop_list);
        delete noun;                        //удаление элемента
    }
}


void delete_ending(Ending* ending)          //очистка списка концовок
{
    if (ending != nullptr)
    {
        if (ending->next_ending != nullptr)     //пока не дошли до конца списка
            delete_ending(ending->next_ending); //рекурсивный вызов
        delete_noun(ending->first_in_column);   //очистка списка существительных с такой концовкой
        delete ending;                          //удаление элемента
    }
}


void clean_struct(Ending*& first_ending)         //очистка структуры
{
    if (first_ending != nullptr)                //если структура не пустая
    {
        delete_ending(first_ending);            //очистка списка концовок
        first_ending = nullptr;                 //сброс указателя на начало структуры
    }
}

int factorial(int n)
{
    if (n == 0)
        return 1;
    int res = 1;
    for (int i = 1; i <= n; i++)
        res *= i;
    return res;
}

int combins(int n, int k) {
    int combs = factorial(n) / (factorial(k) * factorial(n - k));
    return combs;
}


int count_rithms(Ending* first_ending)
{
    int number_of_rithms = 0;
    Ending* curr_ending = first_ending;
    while (curr_ending != nullptr)
    {
        number_of_rithms += combins(curr_ending->counter, 2);
        curr_ending = curr_ending->next_ending;
    }
    return number_of_rithms;
}

void print_word(const Noun* noun, ofstream& f, bool first_in_pair)
{
    if (first_in_pair)
        f << '\n';
    f << "\t" << noun->noun << " повторений - " << noun->repetitions;
    if (noun->repetitions == 1)
        f << " (в предложении " << noun->prop_list->number << ")\t";
    else
    {
        f << " (в предложениях ";
        Prop* curr_prop = noun->prop_list;
        while (curr_prop != nullptr)
        {
            f << curr_prop->number;
            curr_prop = curr_prop->next_prop;
            if (curr_prop != nullptr)
                f << ", ";
        }
        f << ')';
    }
}

void write_to_file(string inp_name, Ending* first_ending)       //формирование выходного файла
{

    string outp_name = "outp_" + inp_name;                       //формирование названия выходного файла
    ofstream out_file(outp_name);                               //создание выходного файла
    if (!out_file.is_open()) {
        cout << "ОШИБКА: Не удалось создать файл " << outp_name << "\n";
        return;
    }

    out_file << "\n\t" << "Пар рифмующихся существительных " << count_rithms(first_ending);

    Ending* curr_ending = first_ending;                         //установка текущей концовки
    while (curr_ending != nullptr)                              //цикл по списку концовок
    {
        Noun* curr_noun = curr_ending->first_in_column;
        if (curr_noun->next_noun != nullptr)                      //проверка, что в структуре больше одного слова с такой концовкой
            while (curr_noun != nullptr)                            //цикл по списку существительных с такой концовкой
            {
                Noun* next_noun = curr_noun->next_noun;
                while (next_noun != nullptr)                          //запись текущего слова в файл в паре с каждым из идущих за ним в списке
                {
                    print_word(curr_noun, out_file, true);
                    print_word(next_noun, out_file, false);
                    next_noun = next_noun->next_noun;
                }
                curr_noun = curr_noun->next_noun;
            }
        curr_ending = curr_ending->next_ending;
    }
    out_file.close();
    cout << "Данные сохранены в файл " << outp_name;
    return;
}

void find_rithms(vector<pair<string, int>>& nouns, vector<string>& transcriptions, string& inp_name)
//void find_rithms(vector<pair<string, int>> nouns, string inp_name)
{
    Ending* first_ending = nullptr;     //указатель на начало структуры

    sort_nouns(nouns, transcriptions, first_ending);
    //sort_nouns(nouns, first_ending, find_flag);

    if (first_ending == nullptr)                                //если структура пуста (нет существительных)
    {
        cout << "Существительных не обнаружено";
        return;
    }    
    if (count_rithms(first_ending)==0)         //если не найдено рифм
    {
        cout << "Рифмующихся существительных не обнаружено";
        clean_struct(first_ending);                             //очистка структуры
        return;
    }
    write_to_file(inp_name, first_ending);
    clean_struct(first_ending);
    return;
}