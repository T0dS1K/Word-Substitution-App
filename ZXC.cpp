#include <iostream>
#include <fstream>
#include <string>
#include <list>
using namespace std;

//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//

void Exit_Message();
void Error_Message();
void Mode_Selection(short& Mode);
string Sym_Remover(string Word);
void Getting_Path(string& pathT);
string Lowercase_Letter(string Word);
string Uppercase_Letter(string Word);
void Standard_Selection(string& Word);
void Procedure_Selection(short& Procedure, const string& Text);
const string Mark = "!@#$%^&*()_+?-=[]{};:'|/.,<>`~";
const string Mark_All = Mark + "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//

class Dictionary
{
public:

    Dictionary();
    Dictionary(const Dictionary& other);
    Dictionary(string Standard_Word, list<string> Synonyms);
    
// Создание нового эталонного слова
    void First_Procedure(list<Dictionary>& dictionary, const string& Text_Word, const short& N)
    {
        dictionary.emplace_back('<' + Text_Word + '>', Synonyms);

        if (N > 0)
        {
            dictionary.pop_back();
        }
    }

    void Second_Third_Procedure(list<Dictionary>& dictionary, const string& Text_Word, const short& vary, const short& N)
    {
        while (true)
        {
            string Word;
            size_t size = 0;

            Standard_Selection(Word);

            for (auto& Unit : dictionary)
            {
                if (Sym_Remover(Unit.Standard_Word) == Word)
                {
                    if (vary == 2) // Добавление к списку синонимов
                    {
                        Unit.Synonyms.emplace_back(Text_Word);
                    }
                    else if (vary == 3) // Замена существующего эталонного слова
                    {
                        Unit.Standard_Word = '<' + Text_Word + '>';
                    }
                    
                    if (N > 0)
                    {
                        if (vary == 2)
                        {
                            Unit.Synonyms.pop_back();
                        }
                        else if (vary == 3)
                        {
                            Unit.Standard_Word = Word;
                        }
                    }

                    break;
                }

                size++;
            }
            if (size == dictionary.size())
            {
                Error_Message();
            }
            else
            {
                break;
            }
        }
    }

// Редактирование текстового документа
    void Word_Replacement(list<string>& Text, list<Dictionary>& dictionary, short& N)
    {
        for (auto& Word : Text)
        {
            const string Stack_Word = Word;

            if (Sym_Remover(Word).size() < 3)
            {
                continue;
            }

            for (const auto& Unit : dictionary)
            {
                if (Lowercase_Letter(Sym_Remover(Word)) == Sym_Remover(Unit.Standard_Word))
                {
                    break;
                }

                for (const auto& Synomym : Unit.Synonyms)
                {
                    if (Lowercase_Letter(Sym_Remover(Word)) == Synomym)
                    {
                        if (Sym_Remover(Word) == Synomym)
                        {
                            Word = Sym_Remover(Unit.Standard_Word);
                        }
                        else
                        {
                            Word = Uppercase_Letter(Sym_Remover(Unit.Standard_Word));
                        }

                        break;
                    }
                }

                if (Word != Stack_Word)
                {
                    string Left_Mark;
                    string Right_Mark;

                    for (size_t size = 0; size != Stack_Word.size(); size++)
                    {
                        if (find(Mark.begin(), Mark.end(), Stack_Word[size]) != Mark.end())
                        {
                            Right_Mark += Stack_Word[size];
                        }
                        else
                        {
                            if (Right_Mark != "")
                            {
                                Left_Mark = Right_Mark;
                                Right_Mark = "";
                            }  
                        }
                    }
                    
                    Word = Left_Mark + Word + Right_Mark;
                    break;
                }
            }

            if (N > 0)
            {
                Word = Stack_Word;
                N--;
            }
        }
    }

    friend void Release_Dictionary(list<Dictionary>& dictionary, const string& pathD);

private:
    string Standard_Word;
    list<string> Synonyms;
};

Dictionary::Dictionary()
{
    Standard_Word = "";
    Synonyms.clear();
}

Dictionary::Dictionary(const Dictionary& other)
{
    this->Standard_Word = other.Standard_Word;
    this->Synonyms = other.Synonyms;
}

Dictionary::Dictionary(string Standard_Word, list<string> Synonyms)
{
    this->Synonyms = Synonyms;
    this->Standard_Word = Standard_Word;
}

//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//

// Заполнение двусвязного списка текстом
void Filling_Text(list<string>& Text, const string& pathT)
{
    ifstream F;
    string Word;

    F.open(pathT);

    while (!F.eof())
    {
        Word = "";
        F >> Word;

        if (Word != "")
        {
            Text.emplace_front(Word);
        }
    }

    F.close();
}

// Удаление дубликатов, однобуквенных слов и символов в словах текста
void NonDuplicate_Text(list<string>& Text)
{
    list<string> Auxiliary;

    for (auto& it : Text)
    {
        it = Lowercase_Letter(Sym_Remover(it));
    }

    while (!Text.empty())
    {
        if (Text.front().size() > 2)
        {
            Auxiliary.emplace_back(Text.front());
        }
        Text.remove(Text.front());
    }

    for (const auto& it : Auxiliary)
    {
        size_t size;

        for (size = 0;size != (it).size(); size++)
        {
            if ((find(Mark_All.begin(), Mark_All.end(), it[size]) != Mark_All.end()))
            {
                break;
            }
        }

        if (size == (it).size())
        {
            Text.emplace_back(it);
        }
        
    }
}

// Очистка текста от слов, которые уже присутствуют в словаре
void Сleaning_Text(list<string>& Text, const string& pathD, const string& pathT)
{
    ifstream F;
    string Word;

    F.open(pathD);
    Filling_Text(Text, pathT);
    NonDuplicate_Text(Text);

    while (!F.eof())
    {
        Word = "";
        F >> Word;

        if (Word != "")
        {
            for (auto it = Text.begin(); it != Text.end(); it++)
            {
                if (*it == Sym_Remover(Word))
                {
                    Text.erase(it);
                    break;
                }
            }
        }
    }
    F.close();
}

// Заполнение двусвязного списка эталонными и синонимическими словами
void Filling_Dictionary(list<Dictionary>& dictionary, const string& pathD)
{
    ifstream F;
    string Word;
    string Standard_Word;
    list<string> Synonyms;

    F.open(pathD);

    while (!F.eof())
    {
        Word = "";
        F >> Word;

        if (Word[0] == '<' or Word == "")
        {
            if (Standard_Word != "")
            {
                dictionary.emplace_back(Standard_Word, Synonyms);
                Synonyms.clear();
            }
            Standard_Word = Word;
        }
        else
        {
            Synonyms.emplace_back(Sym_Remover(Word));
        }
    }

    F.close();
}

// Вывод в файл измененный словарь
void Release_Dictionary(list<Dictionary>& dictionary, const string& pathD)
{
    fstream F;

    F.open(pathD, fstream::out);

    for (const auto& Unit : dictionary)
    {
        F << Unit.Standard_Word;

        for (const auto& Synomym : Unit.Synonyms)
        {
            if (Synomym == Unit.Synonyms.front())
            {
                F << " {" << Synomym;
            }
            else
            {
                F << " " << Synomym;
            }
        }

        if (Unit.Synonyms.size() != 0)
        {
            F << "}\n";
        }
        else
        {
            F << "\n";
        }
    }

    F.close();

}

//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//

int main()
{
    while (true)
    {
        setlocale(LC_ALL, "RU");
        const string pathD = "Dictionary.txt";
        list<Dictionary> dictionary;
        list<string> Text;
        Dictionary ZXC;
        string pathT;
        fstream F;
        short N = 0;
        short Mode = 0;
        short Procedure = 0;

        F.open(pathD);
        if (!F.is_open())
        {
            Error_Message();
            return -1337;
        }
        F.close();

        Getting_Path(pathT);

        F.open(pathT);
        if (!F.is_open())
        {
            Error_Message();
            return -1337;
        }
        F.close();

        Mode_Selection(Mode);
        Filling_Dictionary(dictionary, pathD);

        if (Mode == 1)
        {
            Сleaning_Text(Text, pathD, pathT);

            while (!Text.empty())
            {
                Procedure_Selection(Procedure, Text.front());

                if (Procedure == 1)
                {
                    ZXC.First_Procedure(dictionary, Text.front(), N);
                }
                else
                {
                    if (Procedure == 2)
                    {
                        ZXC.Second_Third_Procedure(dictionary, Text.front(), Procedure, N);
                    }
                    else
                    {
                        if (Procedure == 3)
                        {
                            ZXC.Second_Third_Procedure(dictionary, Text.front(), Procedure, N);
                        }
                        else
                        {
                            if (Procedure == 4)
                            {
                                N++;
                            }
                            else
                            {
                                Exit_Message();
                                return 1337;
                            }
                        }
                    }
                }
                Text.pop_front();
                N--;
            }

            if (N != 0)
            {
                Release_Dictionary(dictionary, pathD);
            }  
        }
        else if (Mode != 2)
        {
            Exit_Message();
            return 1337;
        }

        Filling_Text(Text, pathT);
        ZXC.Word_Replacement(Text, dictionary, N);

        F.open(pathT, fstream::out);

        while (!Text.empty())
        {
            F << Text.back();
            Text.pop_back();

            if (!Text.empty())
            {
                F << " ";
            }
        }

        F.close();
        Exit_Message();   
    }
        
    return 1337;
}

//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//

// Получение пути на файл
void Getting_Path(string& pathT)
{
    cout << "Введите путь на txt файл: \n\n";
    cout << "---> "; cin >> pathT; cout << endl << endl;
}

// Сообщение об ошибке
void Error_Message()
{
    cout << "\t_______________________\n";
    cout << "\tПРОИЗОШЛА ОШИБКА (#> <)\n\n";
}

// Сообщение о завершении программы
void Exit_Message()
{
    cout << "\t_____________________________\n";
    cout << "\tПРОГРАММА УСПЕШНО ЗАВЕРШИЛАСЬ\n\n\n\n";
}

// Первое меню
void Mode_Selection(short& Number)
{
    cout << "Выберите режим работы: \n";
    cout << "\t1. Обучающий\n";
    cout << "\t2. Автоматический\n";
    cout << "\t\t...\n";
    cout << "\t*. Выход\n";
    cout << "---> "; cin >> Number; cout << endl;
}

// Второе меню
void Procedure_Selection(short& Procedure, const string& Text)
{
    cout << "Выберите процедуру для слова " << "| " << Text << " |\n";
    cout << "\t1. Создание нового эталонного слова\n";
    cout << "\t2. Добавление слова к списку синонимов\n";
    cout << "\t3. Замена существующего эталонного слова\n";
    cout << "\t4. Пропустить\n";
    cout << "\t\t\t...\n";
    cout << "\t*. Выход\n";
    cout << "---> "; cin >> Procedure; cout << endl;
}

// Третье меню
void Standard_Selection(string& Word)
{
    cout << "Введите существующее эталонное слово:\n\n";
    cout << "---> "; cin >> Word; cout << endl;
}

// Функция, удаляющая знаки препинания в начале и в конце слова
string Sym_Remover(string Word)
{
    if (Word.size() > 1)
    {
        while (true)
        {
            if (find(Mark.begin(), Mark.end(), Word[0]) != Mark.end())
            {
                Word.erase(0, 1);
            }
            else
            {
                if (find(Mark.begin(), Mark.end(), Word[Word.size() - 1]) != Mark.end())
                {
                    Word.erase(Word.size() - 1, 1);
                }
                else
                {
                    return Word;
                }
            }
        }
    }
    else
    {
        return Word;
    }
    
}

// Изменение первой буквы на маленькую
string Lowercase_Letter(string Word)
{
    if ((int)Word[0] > 64 and (int)Word[0] < 91)
    {
        Word[0] = (char)((int)Word[0] + 32);
    }

    return Word;
}

// Изменение первой буквы на большую
string Uppercase_Letter(string Word)
{
    if ((int)Word[0] > 96 and (int)Word[0] < 123)
    {
        Word[0] = (char)((int)Word[0] - 32);
    }

    return Word;
}