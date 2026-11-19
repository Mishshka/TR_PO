#include "out_work.h"

using namespace std;

long long max_l = 0;
long long max_r = 0;

string get_ending(const string& transcription)
{
    size_t stress_pos = transcription.find("\xCB\x88");
    if (stress_pos == string::npos) return transcription;

    // Находим начало последнего UTF-8 символа в строке
    size_t last_char_start = transcription.size();
    if (last_char_start > 0) {
        last_char_start--;
        while (last_char_start > 0 && (transcription[last_char_start] & 0xC0) == 0x80) {
            last_char_start--;
        }
    }

    // Знак ударения "\xCB\x88" занимает 2 байта, если он находится перед последним символом, ударение падает на последнюю букву.
    if (stress_pos + 2 == last_char_start) {
        // Находим начало предпоследнего UTF-8 символа (буквы перед знаком ударения)
        size_t prev_char_start = stress_pos;
        if (prev_char_start > 0)
        {
            prev_char_start--;
            while (prev_char_start > 0 && (transcription[prev_char_start] & 0xC0) == 0x80) {
                prev_char_start--;
            }
        }

        // Возвращаем подстроку, начиная с предпоследней буквы.
        return transcription.substr(prev_char_start);
    }

    // В остальных случаях возвращаем подстроку от знака ударения до конца
    return transcription.substr(stress_pos);
}

void put_in_struct(pair<string, long long> ex_of_noun, const string& transcription, Ending*& first_ending)
{
    string noun = ex_of_noun.first;
    long long prop_number = ex_of_noun.second;
    string ending = get_ending(transcription);

    Ending* curr_ending = first_ending;
    Ending* prev_ending = nullptr;

    if (first_ending != nullptr)
    {
        while (curr_ending != nullptr)
        {
            if (ending != curr_ending->ending)
            {
                prev_ending = curr_ending;
                curr_ending = prev_ending->next_ending;
            }
            else
            {
                break;
            }
        }
        if (curr_ending == nullptr)
        {
            curr_ending = new Ending;
            curr_ending->ending = ending;
            curr_ending->next_ending = nullptr;
            curr_ending->first_in_column = nullptr;
            prev_ending->next_ending = curr_ending;
        }
    }
    else
    {
        curr_ending = new Ending;
        curr_ending->ending = ending;
        curr_ending->next_ending = nullptr;
        curr_ending->first_in_column = nullptr;
        first_ending = curr_ending;
    }

    Noun* curr_noun = nullptr;
    Noun* prev_noun = nullptr;
    if (curr_ending->first_in_column == nullptr)
    {
        curr_noun = new Noun;
        curr_ending->first_in_column = curr_noun;
    }
    else
    {
        curr_noun = curr_ending->first_in_column;

        while (curr_noun != nullptr)
        {
            if (curr_noun->noun != noun)
            {
                prev_noun = curr_noun;
                curr_noun = prev_noun->next_noun;
            }
            else break;
        }
        if (curr_noun != nullptr)
        {
            curr_noun->repetitions++;
            if (curr_noun->repetitions > max_r) max_r = curr_noun->repetitions;

            Prop* curr_prop = curr_noun->prop_list;
            while (curr_prop->next_prop != nullptr)
                curr_prop = curr_prop->next_prop;
            Prop* add_prop = new Prop;
            add_prop->number = prop_number;
            add_prop->next_prop = nullptr;
            curr_prop->next_prop = add_prop;
            return;
        }
        curr_noun = new Noun;
        if (prev_noun != nullptr)
        {
            prev_noun->next_noun = curr_noun;
        }
    }
    curr_ending->counter++;
    curr_noun->prop_list = new Prop;
    curr_noun->prop_list->number = prop_number;
    curr_noun->prop_list->next_prop = nullptr;
    curr_noun->repetitions = 1;
    curr_noun->noun = noun;
    if (noun.length() > max_l) max_l = noun.length();
    curr_noun->next_noun = nullptr;
}

void sort_nouns(const vector<pair<string, long long>>& nouns, const vector<string>& transcriptions, Ending*& first_ending)
{
    for (long long i = 0; i < nouns.size(); i++)
    {
        put_in_struct(nouns[i], transcriptions[i], first_ending);
    }
}

void clear_prop_list(Prop* prop)
{
    while (prop != nullptr)
    {
        Prop* next_prop = prop->next_prop;
        delete prop;
        prop = next_prop;
    }
}

void delete_noun(Noun* noun)
{
    while (noun != nullptr)
    {
        Noun* next_noun = noun->next_noun;
        clear_prop_list(noun->prop_list);
        delete noun;
        noun = next_noun;
    }
}

void delete_ending(Ending* ending)
{
    while (ending != nullptr)
    {
        Ending* next_ending = ending->next_ending;
        delete_noun(ending->first_in_column);
        delete ending;
        ending = next_ending;
    }
}

void clean_struct(Ending*& first_ending)
{
    delete_ending(first_ending);
    first_ending = nullptr;
}

long long combins(long long n, long long k) {
    if (k > n)
        return 0;

    if (k == 2)
    {
        if (n % 2 == 0)
            return (n / 2) * (n - 1);
        else
            return n * ((n - 1) / 2);
    }

    return 0;
}

long long count_rithms(Ending* first_ending)
{
    long long number_of_rithms = 0;
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
    f << (first_in_pair ? "\tПервое слово: " : "\tВторое слово: ")
        << noun->noun << '\n';
    f << "\tКоличество повторений: " << noun->repetitions << '\n';
    f << "\tНомера предложений: ";

    Prop* curr_prop = noun->prop_list;
    if (curr_prop == nullptr)
    {
        f << "нет данных";
    }
    else
    {
        while (curr_prop != nullptr)
        {
            f << curr_prop->number;
            curr_prop = curr_prop->next_prop;
            if (curr_prop != nullptr)
                f << ", ";
        }
    }

    f << "\n";
}

void write_to_file(string inp_name, Ending* first_ending)
{
    string base_outp_name = "outp_" + inp_name;
    string final_name = base_outp_name;
    int counter = 1;

    // Проверяем существование файла и генерируем новое имя с порядковым номером при коллизии
    while (true) {
        ifstream f(final_name);
        if (!f.good()) {
            break; // Файл не существует, можно использовать это имя
        }
        f.close();

        size_t dot_pos = base_outp_name.find_last_of('.');
        if (dot_pos != string::npos) {
            final_name = base_outp_name.substr(0, dot_pos) + "_" + to_string(counter) + base_outp_name.substr(dot_pos);
        }
        else {
            final_name = base_outp_name + "_" + to_string(counter);
        }
        counter++;
    }

    ofstream out_file(final_name);
    if (!out_file.is_open()) {
        cout << "ОШИБКА: Не удалось создать файл " << final_name << "\n";
        return;
    }

    out_file << "\n\t" << "Пар рифмующихся существительных: " << count_rithms(first_ending) << "\n\n";

    long long pair_number = 0;

    Ending* curr_ending = first_ending;
    while (curr_ending != nullptr)
    {
        Noun* curr_noun = curr_ending->first_in_column;
        if (curr_noun->next_noun != nullptr)
        {
            while (curr_noun != nullptr)
            {
                Noun* next_noun = curr_noun->next_noun;
                while (next_noun != nullptr)
                {
                    ++pair_number;
                    out_file << "\n\tПАРА № " << pair_number << "\n";
                    print_word(curr_noun, out_file, true);
                    print_word(next_noun, out_file, false);
                    next_noun = next_noun->next_noun;
                }
                curr_noun = curr_noun->next_noun;
                out_file << endl;
            }
        }
        curr_ending = curr_ending->next_ending;
    }
    out_file.close();
    cout << "\nДанные сохранены в файл " << final_name;
    return;
}

void find_rithms(vector<pair<string, long long>>& nouns, vector<string>& transcriptions, string& inp_name)
{
    Ending* first_ending = nullptr;

    //точка начала формирования структуры
    auto t1 = chrono::steady_clock::now();

    sort_nouns(nouns, transcriptions, first_ending);

    //точка окончания структурирования
    auto t2 = chrono::steady_clock::now();

    if (first_ending == nullptr)
    {
        cout << "Существительных не обнаружено";
        return;
    }
    if (count_rithms(first_ending) == 0)
    {
        cout << "Рифмующихся существительных не обнаружено";
        clean_struct(first_ending);
        return;
    }

    //точка начала записи файла
    auto t3 = chrono::steady_clock::now();

    write_to_file(inp_name, first_ending);

    //точка окончания записи
    auto t4 = chrono::steady_clock::now();

    clean_struct(first_ending);

    //точка окончания чистки
    auto t5 = chrono::steady_clock::now();

    cout << "\n\nВремя структурирования: " << chrono::duration_cast<chrono::milliseconds>(t2 - t1) << endl;
    cout << "Время  записи файла: " << chrono::duration_cast<chrono::milliseconds>(t4 - t3) << endl;
    cout << "Время очистки: " << chrono::duration_cast<chrono::milliseconds>(t5 - t4) << endl << endl;

    return;
}