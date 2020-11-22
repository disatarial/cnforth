
#define     CELL        long  // Определить тип данных с той же шириной, что и тип указателя в 32-битных и 64-битных системах
#define     STACK_LEN   1024  //Определение глубины стека
#define     BUFF_LEN    1024  //Длина буфера
#define     TRUE        1
#define     FALSE       0
#define     EXPLAIN     0     //Режим интерпретации
#define     COMPILE     1     //Режим компиляции
#define     REVEAL_WORD 0     //Пометить как отображаемое слово
#define     IMMD_WORD   1     //Отметить как немедленное слово
#define     HIDE_WORD   2     //Пометить как скрытое слово

#define DEBUG 0
#if DEBUG
    #define PRINT(fmt, args...) printf(fmt,##args);
#else
    #define PRINT(fmt, args...)
#endif


//Указатель функции кодового домена
typedef void(*fn_p)();  


//Определение структуры слова Forth со структурой, реализация словаря с использованием связанного списка
typedef struct Word
{
    struct Word *link;     //Forth Поле ссылки для слова
    CELL flag;             //Forth Количество тегов для идентификации немедленных слов, скрытых слов
    char *name;            //Forth Доменное имя слова
    fn_p code_p;           //Forth Кодовое поле слова
    struct Word **wplist;  //Forth Поле параметров слова
} Word;


//Определение структуры словаря
typedef struct Dict
{
    CELL size;               //Forth Количество слов в словаре
    Word *head;              //Forth Адрес последнего слова в списке словарей
    Word *wplist_tmp[BUFF_LEN];// Сохраняет поле параметра расширенного слова, определяемое режимом компиляции, временно
} Dict;


//Forth Указатель ядра во время работы системы
CELL state;                  //Forth Переменные состояния
char forth_text[BUFF_LEN];   //Forth Буфер текста кода
char *current_text;          //Forth Указатель на начало текущего слова
char *text_p;                //Forth Указатель текста кода
Dict *forth_dict;            //Forth Указатель словаря
CELL DS[STACK_LEN];          // Стек параметров
CELL RS[STACK_LEN];          //  Возврат к стеку
CELL *DP, *RP;               // Указатель стека
Word *IP_list[BUFF_LEN];     // Список инструкций режима интерпретации, длина BUFF_LEN  
Word **IP;                   // Указатель списка инструкций (указатель на указатель) // ??стек для списка слов??. 
Word **IP_head;              //IPlist Выберите указатель, указывающий на другой список инструкций в зависимости от переменной состояния

// Разбор текста
int check_blank(char c);  // Определяет, является ли символ пробела
char *parse_word();  // Возвращает текущее слово forth во входном потоке и обновляет указатель text_p

//Forth Функции построения слов
Word *create(char *name, fn_p  fp); //Создать домен имени Forth Word
void does(Word *c, Word **list, int n); // Forth Построение параметрических полей в слове  
Word *def_core(char *name, fn_p  fp); // Forth Создание основного слова
void colon_code();  // Кодовое поле расширенного слова
void cons_code();   // Кодовые поля постоянных слов
void var_code();    // Кодовые поля переменных слов

//Forth Операционные функции словаря
Dict *dict_init();
int dict_ins_next(Dict *dict, Word *word);
Word *dict_search_name(Dict *dict, char *name);
void dict_destroy_word(Word *word);
int dict_rem_after(Dict *dict, char *name);

//Forth Функция действия списка инструкций
void explain(); //IP Выполнение списка
int is_num(char *s); // Определяет, является ли строка числовой
int find(Dict *dict, char *name); // В зависимости от имени слова перейдите к соответствующему списку IP-адресов

//Forth Функция действия стека
void empty_stack();
void stack_error(int n);
void ip_push(Word *w, Word** list);  //IP хранилище PUSH
void ds_push(CELL n);
void rs_push(CELL n);
CELL ds_pop();
CELL rs_pop();
CELL ds_top();
CELL rs_top();

//Forth Основные слова
void lit();      // (lit)
void popds();    // .
void bye();      // bye

void ret();      // ret

void depth();    // depth
void add();      // +
void sub();      // -
void mul();      // *
void divv();     // /

void drop();     // drop
void showds();   // .s
void pick();     // pick
void roll();     // roll

void invar();     // !
void outvar();    // @

void equal();     // =
void noequal();   // <>
void morethan();  // >
void lessthan();  // <

void if_branch();  // ?branch
void branch();     // branch

void doo();        // (do)
void loopp();      // (loop)

void tor();        // >r
void rto();        // r>
void rat();        // r@

void emit();       // emit
void words();      // words

void immediate();  // immediate
void compile();    // compile  Скомпилируйте код времени выполнения, который следует за словом
void compile_s();  // ,  Число вершин стека компиляции в определении расширенного слова
void pushds_cfa(); // ' Нажатие адреса выполнения последнего слова в стек данных

//Forth Непосредственные слова в ядре
void in_interpret(); // [
void out_interpret();// ]
void myself();     // myself
void defcolon();   // :
void endcolon();   // ;
void _if();        // if
void _else();      // else
void _then();      // then
void _do();        // do
void _loop();      // loop
void see();        // see
void forget();     // forget
void var();        // variable
void cons();       // constant
void load();       // load


//Forth Часть интерпретатора
void interpret();
int load_file(char *file_path);