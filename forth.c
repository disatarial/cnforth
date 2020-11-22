#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "forth.h"

int check_blank(char c)
{
    return (c==' ' || c=='\t' || c=='\n');
}


char *parse_word()
{
    char *now;
    while (check_blank(*text_p)) //Вернуть указатель после пропуска пустых символов в начале строки
        text_p++;
    
    now = text_p;
    
    while ( !check_blank(*text_p)  && (*text_p)!='\0') //Пропустить первое слово в строке
        text_p++;
    
    if(*text_p == '\0') 
        return now;
    *text_p = '\0';   //Измените пробел после первого слова в строке на «\ 0»
    text_p++;
    
    return now;
}

// создание словарной статьи
Word *create(char *name, fn_p  fp)  
{
    Word *w=(Word*)malloc(sizeof(Word)); // выделение места для адреса слова
    w->code_p=fp; // добавление
    
    w->name=(char*)malloc(strlen(name) + 1); // выделение места для имени
    strcpy(w->name,name); // копироване 
    
    w->wplist=NULL;   

    w->flag = HIDE_WORD;  // ? скрытое слово? l для  создания нового слова? чтоб оно не появлялось в словаре?

    return w;
}

// создание выполняемой части слова
void does(Word *w, Word **list, int n)
{
    if(n != 0) {
        w->wplist = (Word**)malloc(n); // выделяем память под выполняему часть слова
        memcpy(w->wplist,list, n); // копируем 
    } else {
        w->wplist = list; 
    }
    w->flag = REVEAL_WORD; // разрешаем поиск слова в словаре
}

// ?связь нового синного слова со словарем? 

Word *def_core(char *name, fn_p  fp) 
{
    Word *w=create(name, fp);
    w->flag = REVEAL_WORD;
    return w;
}


void colon_code()
{
    RP++;
    *RP=(CELL)IP;
    IP=(*IP)->wplist-1;
    PRINT("[DEBUG] go_to_function \n") // 进入子例程 Войдите в подпрограмму
}


void cons_code()
{
    ds_push((CELL)((*IP)->wplist));
}


void var_code()
{
    ds_push((CELL)*IP);
}

// инициализация словаря
Dict *dict_init()
{
    Dict *dict=(Dict*)malloc(sizeof(Dict)); // выделение памяти под словарь
    dict->size = 0; // обнуление размера
    dict->head = NULL; // обнуление заголовка
    return dict;
}

// связываение словаря и   слова  (сишного) .
int dict_ins_next(Dict *dict, Word *word)
{
    word->link = dict->head; 
    dict->head = word;
    dict->size++;
    return 0;
}

// поиск в словаре . как наблюдаю  отсутствует список словарей и есть только 1 словарь!!! 
Word *dict_search_name(Dict *dict, char *name)
{
    Word *w = dict->head;
    //为了支持递归忽略隐藏词 Игнорировать скрытые слова для поддержки рекурсии
    while ((w != NULL && strcmp(w->name,name))
        || (w != NULL && !strcmp(w->name,name) && w->flag == HIDE_WORD))
    {  
        w=w->link;
    }
    
    return w;
}

// ? похоже на удаление слова  из словаря?
void dict_destroy_word(Word *word)
{
    free(word->name);
    if(word->code_p == colon_code) free(word->wplist);
    free(word);
}

// удаление слова в словаре и всех последующих
int dict_rem_after(Dict *dict, char *name)
{
    Word *w = dict_search_name(dict, name);
    Word *del_w;
    
    if(w == NULL)
    {
        printf("%s :\n\tCan't find!\n", name);
    }
    else
    {
        if(w->wplist == NULL)
        {
            printf("\tCore Word can't be deleted\n");
            return 0;
        }
        do
        {
            del_w = dict->head;
            dict->head = dict->head->link;
            dict_destroy_word(del_w);
            dict->size--;
        } while(del_w != w);
        
        return 1;
    }
    
    return 0;
}

// ?режим интерпретации? или это команда на интерпретацию?
void explain()
{
    Word  **IP_end = IP;
    IP=IP_head;
    
    while(IP != IP_end)
    {
        PRINT("[DEBUG]interpret> %s\n", (*IP)->name) // Интерпретация>
        
        (*IP)->code_p();
        ++IP;
    }
    IP_head = IP;
}

// проверка на цифру
int is_num(char *s)
{
    if(*s == '-')
        s++;

    while (*s != 0)
    {
        if (!isdigit((CELL)*s)) 
            return 0;
        s++;
    }
    return 1;
}

// поиск в словаре
int find(Dict *dict, char *name)
{
    Word *word_p;
    word_p = dict_search_name(dict, name); // попытка поиска
    
    if(!strcmp(":", name) || !strcmp("]", name)) state = COMPILE; // переключится в режим компиляции если есть команда  : или ]
    
    if(state == COMPILE)
    {
        if(word_p==NULL)    //Словарь связанный список не может найти имя, чтобы определить, является ли это числом
        {
            if (!is_num(name))    
            {
                return 0;    //如果不是数字，返回0   Возвращает 0, если это не число
            }
            else 
            {               //如果是数字 Если это число
                PRINT("[DEBUG] number_ok %s\n",name) //Успешно найти номер%
                ip_push(dict_search_name(dict, "(lit)"), IP_head);   //Поместите указатель основного слова push в массив IP     
                ip_push((Word*)(CELL)(atoi(name)), IP_head);    //Приведение числа типа CELL к типу указателя Word

                return 1;
            }            
        }
        else if(word_p->flag == IMMD_WORD)  //Немедленное слово
        {
            PRINT("[DEBUG] immediate_run %s\n", name) // Выполнение немедленного слова
            if(word_p->wplist != NULL)
            {
                in_interpret();
                ip_push(word_p, IP_head);
                explain();
                out_interpret();
            }
            else
            {
                word_p->code_p();
            }
        }
        else
        {
            PRINT("[DEBUG]word_%s_ok\n",name) // Слова%s успешно скомпилированы
            ip_push(word_p, IP_head);
        }
    }
    else if(state == EXPLAIN)
    {
        if(word_p==NULL)    //Словарь связанный список не может найти имя, чтобы определить, является ли это числом
        {
            if (!is_num(name))    
            {
                return 0;    //Возвращает 0, если это не число
            }
            else 
            {               //Если это число
                PRINT("[DEBUG]number ok%s\n",name)
                ds_push((CELL)(atoi(name)));

                return 1;
            }            
        }
        else
        {
            PRINT("[DEBUG]word ok\n",name)
            ip_push(word_p, IP_head);
            explain();
        }
    }

    return 1;
}

// ? очистка стека?
void empty_stack()
{
    DP=DS-1;
    RP=RS-1;
}

// ?вывод ошибки стеков? (1- пуст\2- переполнен)
void stack_error(int n)
{
    switch(n)
    {
        case 1: printf("Stack underflow\n"); break;
        case 2: printf("Stack overflow\n"); break;
    }
    exit(0);
}


// положить слово в стек IP 
void ip_push(Word *w, Word** list)
{
    if(IP >= list+BUFF_LEN){stack_error(2);}
    *IP=w;
    IP++;
}


void ds_push(CELL n)
{
    if(DP >= DS+STACK_LEN-1){stack_error(2);}
    DP++;
    *DP=n;
}


void rs_push(CELL n)
{
    if(RP >= RS+STACK_LEN-1){stack_error(2);}
    RP++;
    *RP=n;
}


CELL ds_pop()
{
    if(DP <= DS-1){stack_error(1);}
    DP--;
    return *(DP+1); 
}


CELL rs_pop()
{
    if(RP <= RS-1){stack_error(1);}
    RP--;
    return *(RP+1); 
}

//обнулить стек данных
CELL ds_top()
{
    if(DP <= DS-1){stack_error(1);}
    return *DP;
}


CELL rs_top()
{
    if(RP <= RS-1){stack_error(1);}
    return *RP;
}

//  положить цыфру в стек данных 
void lit()
{
    IP++;
    ds_push((CELL)*IP);
    PRINT("[DEBUG] number % ld data stack\n", (CELL)*IP) // Количество % ld в стек данных
}

// вывести  число из стека данных
void popds()
{
    printf("%ld\n", ds_pop());
}

// выход из программы
void bye()
{
    exit(1);
}

// возврат из подпрограммы
void ret()
{
    IP=(Word**)(rs_pop());
    PRINT("[DEBUG]ret \n") // возврат из подпрограммы
}

// грубина стека данных
void depth()
{
    ds_push((CELL)(DP-DS+1));
}

// сложение
void add()
{
    ds_push(ds_pop() + ds_pop());
}

// вычитание
void sub()
{
    CELL d = ds_pop();
    ds_push(ds_pop() - d);
}

// умножение
void mul()
{
    ds_push(ds_pop() * ds_pop());
}

// деление
void divv()
{
    CELL d = ds_pop();
    ds_push(ds_pop() / d);
}

// удаление
void drop()
{
    ds_pop();
}

// показать стек данных
void showds()
{
    printf("<%ld> ", (CELL)(DP-DS+1));
    CELL *i=DS;
    for (;i<=DP ;i++ )
    {
        printf("%ld ",*i);
    }
    printf("\n");
}


void pick()
{
    CELL k = ds_pop();
    if(DP-k+1 <= DS-1){stack_error(1);}
    ds_push(*(DP-k+1));
}


void roll()
{
    CELL k = ds_pop();
    if(DP-k+1 <= DS-1){stack_error(1);}
    CELL dk = *(DP-k+1);
    for(; k>1; k--) {
        *(DP-k+1) = *(DP-k+2);
    }
    ds_pop();
    ds_push(dk);
}

// запомнить переменную ( ! )
void invar() 
{
    Word *p = (Word *)(ds_pop());
    p->wplist = (Word **)ds_pop();
}

// получить переменную ( @ )
void outvar() 
{
    Word *p = (Word *)(ds_pop());
    ds_push((CELL)(p->wplist));
}

// ( = )
void equal()
{
    if(ds_pop() == ds_pop())
    {
        ds_push(-1);
    }
    else
    {
        ds_push(0);
    }
}

// <>
void noequal()
{
    if(ds_pop() != ds_pop())
    {
        ds_push(-1);
    }
    else
    {
        ds_push(0);
    }
}


void morethan()
{
    CELL d = ds_pop();
    if(ds_pop() > d)
    {
        ds_push(-1);
    }
    else
    {
        ds_push(0);
    }
}


void lessthan()
{
    CELL d = ds_pop();
    if(ds_pop() < d)
    {
        ds_push(-1);
    }
    else
    {
        ds_push(0);
    }
}

// ?branch
void if_branch()
{
    if(ds_pop() == 0)
    {
        IP = IP + (CELL)(*(IP+1));
    }
    else
    {
        IP++;
    }
}


void branch()
{
    IP = IP + (CELL)(*(IP+1));
}


void doo()
{
    CELL index = ds_pop();
    CELL limit = ds_pop();
    if(limit <= index)
    {
        IP = IP + (CELL)(*(IP+1)); 
    }
    else
    {
        IP++;
        index++;
        rs_push(index);
        rs_push(limit);
    }
}


void loopp() 
{
    IP = IP - (CELL)(*(IP+1)); 
    ds_push(rs_pop());
    ds_push(rs_pop());
}


void tor()
{
    rs_push(ds_pop());
}


void rto()
{
    ds_push(rs_pop());
}


void rat()
{
    ds_push(rs_top());
}


void emit()
{
    putchar((char)(ds_pop()));
}


void words()
{
    Word *w = forth_dict->head;
    while (w != NULL)
    {  
        printf("%s ", w->name);
        w=w->link;
    }
    printf("\n");
}


void immediate()
{
    forth_dict->head->flag = IMMD_WORD;
}


void pushds_cfa()
{
    current_text = parse_word();
    ds_push((CELL)dict_search_name(forth_dict, current_text));
}


void compile_wplist()
{
    IP++;
    Word **tmp = IP;
    Word *word_p = *IP;
    Word **IP_over = (Word **)rs_pop();
    IP = (Word **)rs_pop();
    if(word_p->wplist != NULL)
    {
        Word **p = word_p->wplist;
        Word *end = dict_search_name(forth_dict, "ret");
        for (; *p != end; p++)
        {
            ip_push(*p, forth_dict->wplist_tmp);
        }
    }
    else
    {
        PRINT("[DEBUG]compiling_word%s\n", word_p->name) // 编译核心词  Компиляция основных слов
        ip_push((Word *)word_p, forth_dict->wplist_tmp);
    }
    rs_push((CELL)IP);
    rs_push((CELL)IP_over);
    IP = tmp;
}


void compile_s()
{
    Word **tmp = IP;
    Word **IP_over = (Word **)rs_pop();
    IP = (Word **)rs_pop();
    CELL num = ds_pop();
    PRINT("[DEBUG]number_data_stack %ld\n", num) // 编译栈顶数 Число вершин стека компиляции
    ip_push((Word *)num, forth_dict->wplist_tmp);
    rs_push((CELL)IP);
    rs_push((CELL)IP_over);
    IP = tmp;
}


void in_interpret()
{
    state = EXPLAIN;
    IP_head = IP_list;
    rs_push((CELL)IP);
    IP=IP_head;
}


void out_interpret()
{
    IP_head = forth_dict->wplist_tmp;
    IP = (Word **)rs_pop();
    state = COMPILE;
}


void myself()
{
    ip_push(forth_dict->head, IP_head);
}


void defcolon()
{
    IP_head = forth_dict->wplist_tmp;
    IP=IP_head;
    current_text = parse_word();
    dict_ins_next(forth_dict, create(current_text, colon_code));
}


void endcolon()
{
    ip_push(dict_search_name(forth_dict, "ret"), IP_head);
    int n = (CELL)IP - (CELL)IP_head;
    does(forth_dict->head, IP_head, n);
    
    //DEBUG模式下打印出IP指针列表 Распечатайте список указателей IP в режиме
    if(DEBUG) {
        printf("[DEBUG]IP_list>  "); // 指针列表 Список указателей>
        Word **p=IP_head;
        for (;p<IP ;p++ )
        {
            printf("%ld ",(CELL)(*p));
        }
        printf("\n");
        showds();
    }
    
    IP_head = IP_list;
    IP=IP_head;
    state = EXPLAIN;
}


void _if()
{
    ip_push(dict_search_name(forth_dict, "?branch"), IP_head);
    rs_push((CELL)IP);
    ip_push((Word *)0, IP_head);
}


void _else()
{
    ip_push(dict_search_name(forth_dict, "branch"), IP_head);
    Word** else_p = IP;
    Word** if_p = (Word**)(rs_pop());
    rs_push((CELL)else_p);
    *if_p = (Word*)(IP - if_p + 1);
    ip_push((Word *)0, IP_head);
}


void _then()
{
    Word** branch_p = (Word**)(rs_pop());
    *branch_p = (Word*)(IP - branch_p); 
}


void _do()
{
    ip_push(dict_search_name(forth_dict, "(do)"), IP_head);
    rs_push((CELL)IP);
    ip_push((Word *)0, IP_head);
    
}


void _loop()
{
    ip_push(dict_search_name(forth_dict, "(loop)"), IP_head); 
    Word** do_p = (Word**)(rs_pop());
    *do_p = (Word*)(IP - do_p + 1); 
    ip_push((Word*)(IP - do_p + 1), IP_head); 
}

// ? показать слово?
void see()
{
    current_text = parse_word();
    Word *word_p = dict_search_name(forth_dict, current_text);
    
    if(word_p == NULL)
    {
        printf("%s :\n\tCan't find!\n", current_text);
    }
    else
    {   //反编译wplist，得出扩展词的字符串定义   Декомпилируйте wplist, чтобы получить определение строки расширенного слова
        printf("%s :\n\t", current_text);
        if(word_p->code_p == colon_code)
        {
            Word **p = word_p->wplist;
            Word *end = dict_search_name(forth_dict, "ret");
            Word *dict_p = forth_dict->head;
            for(; *p != end; p++)
            {
                while (dict_p != NULL && dict_p != *p)
                {  
                    dict_p=dict_p->link;
                }

                if(dict_p != NULL)
                    printf("%s ", (*p)->name);
                else
                    printf("%ld ", (CELL)(*p));
                dict_p = forth_dict->head;
            }
            printf(";");
            if(word_p->flag == IMMD_WORD)
                printf(" immediate\n");
            else
                printf("\n");
        }
        else
        {
            printf("%s\n", word_p->name);
        }
    }
}


void forget()
{
    current_text = parse_word();
    dict_rem_after(forth_dict, current_text); //删除当前扩展词以及词典中该词之后定义的所有扩展词 Удаляет текущее расширенное Слово и все расширенные слова, определенные после этого слова в словаре
}


void var()
{
    current_text = parse_word();
    dict_ins_next(forth_dict, create(current_text, var_code));
    does(forth_dict->head, (Word **)0, 0);
}


void cons()
{
    current_text = parse_word();
    dict_ins_next(forth_dict, create(current_text, cons_code));
    does(forth_dict->head, (Word **)ds_pop(), 0);
    
}


void load()
{
    current_text = parse_word();
    load_file(current_text);
}


void interpret()
{
    state = EXPLAIN;
    text_p = forth_text;
    IP_head = IP_list;
    IP=IP_head;
       
    while (*(current_text = parse_word()) != '\0')
    {
        if(!strcmp(".\"",current_text))  //如果是." str " 则立即编译其中的字符串str  Если да."str" немедленно компилирует строку, в которой str
        {
            PRINT("[DEBUG] string \n") // 编译字符串 Строка компиляции

            char tempstr[BUFF_LEN]; 
            while(*text_p != '\"')
            {
                sprintf(tempstr, "%ld", (CELL)(*text_p));
                find(forth_dict, tempstr);
                find(forth_dict, "emit");
                text_p++;
            }
            text_p++;
        }
        else if(!strcmp("(",current_text))  //注释模式 Режим аннотации
        {

            while(*text_p != ')')
            {
                text_p++;
            }
            text_p++;
        }
        else if(!find(forth_dict, current_text))
        {
            printf("word [%s] not found! \n",current_text);
            empty_stack();
            IP=IP_head;
            return;
        }
    }
}


// Чтение кода Forth из внешнего файла
int load_file(char *file_path)
{
    FILE *fp; //文件指针 Указатель на файл
    char c;
    int i = 0;
    int colon_flag = FALSE;

    if((fp = fopen(file_path, "r")) == NULL)
    {
        printf("Can't open %s\n", file_path);
        return 0;
    }
    
    do
    {
        c = getc(fp);
        if((c != '\n' && c != EOF) 
            || (c == '\n' && colon_flag == TRUE))
        {
            if(c == ':') colon_flag = TRUE;
            else if(c == ';') colon_flag = FALSE;
            forth_text[i] = c;
            i++;
        }
        else if((c == '\n' && colon_flag == FALSE)
            || c == EOF)
        {
            forth_text[i] = '\0';
            interpret();
            i = 0;
        }           
    } while(c != EOF);
    fclose(fp);

    return 1;
}


// Главный вход программы
int main(int argc, char *argv[]) 
{
    empty_stack();
    IP_head = IP_list;
    IP = IP_head;
    forth_dict= dict_init();
    
    //Инициализация словаря
    dict_ins_next(forth_dict, def_core("(LIT)",lit));
    dict_ins_next(forth_dict, def_core(".",popds));
    dict_ins_next(forth_dict, def_core("BYE",bye));
    dict_ins_next(forth_dict, def_core("RET",ret));
    dict_ins_next(forth_dict, def_core("DEPTH",depth));
    dict_ins_next(forth_dict, def_core("+",add));
    dict_ins_next(forth_dict, def_core("-",sub));
    dict_ins_next(forth_dict, def_core("*",mul));
    dict_ins_next(forth_dict, def_core("/",divv));
    dict_ins_next(forth_dict, def_core("DROP",drop));
    dict_ins_next(forth_dict, def_core(".S",showds));
    dict_ins_next(forth_dict, def_core("PICK",pick));
    dict_ins_next(forth_dict, def_core("ROLL",roll));
    dict_ins_next(forth_dict, def_core("!", invar));
    dict_ins_next(forth_dict, def_core("@", outvar));
    dict_ins_next(forth_dict, def_core("=",equal));
    dict_ins_next(forth_dict, def_core("<>",noequal));
    dict_ins_next(forth_dict, def_core(">",morethan));
    dict_ins_next(forth_dict, def_core("<",lessthan));
    dict_ins_next(forth_dict, def_core("?BRANCH",if_branch));
    dict_ins_next(forth_dict, def_core("BRANCH",branch));
    dict_ins_next(forth_dict, def_core("(DO)",doo));
    dict_ins_next(forth_dict, def_core("(LOOP)",loopp));
    dict_ins_next(forth_dict, def_core(">R",tor));
    dict_ins_next(forth_dict, def_core("R>",rto));
    dict_ins_next(forth_dict, def_core("R@",rat));
    dict_ins_next(forth_dict, def_core("EMIT", emit));
    dict_ins_next(forth_dict, def_core("WORDS",words));
    dict_ins_next(forth_dict, def_core("IMMEDIATE",immediate));
    dict_ins_next(forth_dict, def_core("COMPILE", compile_wplist)); 
    dict_ins_next(forth_dict, def_core(",", compile_s));
    dict_ins_next(forth_dict, def_core("'", pushds_cfa));
    
    dict_ins_next(forth_dict, def_core("[",in_interpret)); immediate();
    dict_ins_next(forth_dict, def_core("]",out_interpret)); immediate();
    dict_ins_next(forth_dict, def_core("MYSELF", myself)); immediate();
    dict_ins_next(forth_dict, def_core(":",defcolon)); immediate();
    dict_ins_next(forth_dict, def_core(";",endcolon)); immediate();
    dict_ins_next(forth_dict, def_core("IF",_if)); immediate();
    dict_ins_next(forth_dict, def_core("ELSE",_else)); immediate();
    dict_ins_next(forth_dict, def_core("THEN",_then)); immediate();
    dict_ins_next(forth_dict, def_core("DO",_do)); immediate();
    dict_ins_next(forth_dict, def_core("LOOP",_loop)); immediate();
    dict_ins_next(forth_dict, def_core("SEE",see)); immediate();
    dict_ins_next(forth_dict, def_core("FORGET",forget)); immediate();
    dict_ins_next(forth_dict, def_core("VARIABLE",var)); immediate();
    dict_ins_next(forth_dict, def_core("CONSTANT",cons)); immediate();
    dict_ins_next(forth_dict, def_core("LOAD",load)); immediate();
    
    
    for(; argc > 1; argc--)
        load_file(*++argv);

    while (1)
    {
        printf(">>> ");
        gets(forth_text);
        interpret();
    }

    
    return 0;
}
