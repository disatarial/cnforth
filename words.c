#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "words.h"


Dict *dict_init()
{
    Dict *dict=(Dict*)malloc(sizeof(Dict));
    dict->size = 0;
    dict->head = NULL;
    
    return dict;
}


int dict_ins_next(Dict *dict, Word *word)
{
    word->next = dict->head;
    dict->head = word;
    dict->size++;
    
    return 0;
}


Word *dict_search_name(Dict *dict, char *name)
{
    Word *w = dict->head;
    while (w != NULL && strcmp(w->name,name))
    {  
        w=w->next;
    }
    
    return w;
}


Word *code(char*name, fnP  fp)
{
    Word *w=(Word*)malloc(sizeof(Word));
    w->fn=fp;
    w->wplist=NULL;   
    w->name=name;

    return w;
}


void dolist()
{
    RP++;
    *RP=(CELL)IP;
    IP=(*IP)->wplist-1;
    PRINT("[DEBUG]进入dolist\n")
}


void change_colon(Word *c, Word **list, int n)
{
    free(c->wplist);
    c->wplist = (Word**)malloc(n);
    memcpy(c->wplist,list, n);
}


Word *colon(char*name, char*str, Word **list, int n)
{
    Word *w=(Word*)malloc(sizeof(Word));
    w->fn=dolist;

    w->name=(char*)malloc(strlen(name)+1);
    strcpy(w->name,name);

    w->wplist=(Word**)malloc(n);
    memcpy(w->wplist,list, n);
    
    w->str=(char*)malloc(strlen(str)+1);
    strcpy(w->str,str);
    
    return w;
}


Word *variable(char*name, char*str, CELL num)
{
    Word *w=(Word*)malloc(sizeof(Word));
    w->fn=NULL;
    
    w->name=(char*)malloc(strlen(name)+1);
    strcpy(w->name,name);
       
    w->num = num;
    
    w->str=(char*)malloc(strlen(str)+1);
    strcpy(w->str,str);
    
    return w;
}


void empty_stack()
{
    DP=DS-1;
    //*DP=0;
    RP=RS-1;
    //*RP=0;
    TP=TS-1;
    //*TP=0;
}


//核心词
void push()
{
    IP++;
    DP++;
    *DP=(CELL)*IP;
}


void popds()
{
    printf("%ld\n", *DP);
    DP--;
}


void bye()
{
    exit(0);
}


void ret()
{
    IP=(Word**)*RP;
    RP--;
}


void putcr()
{
    putchar('\n');
}


void depth()
{
    CELL d = DP - DS + 1;
    DP++;
    *DP = d; 
}


void add()
{
    *(DP-1)=*(DP-1)+(*DP);
    DP--;
}


void sub()
{
    *(DP-1)=*(DP-1)-(*DP);
    DP--;
}


void mul()
{
    *(DP-1)=*(DP-1)*(*DP);
    DP--;
}


void divv()
{
    *(DP-1)=*(DP-1)/(*DP);
    DP--;
}


void dup()
{
    DP++;
    *DP=*(DP-1);
}


void swap()
{
    CELL t=*DP;
    *DP=*(DP-1);
    *(DP-1)=t;
}


void over()
{
    *(DP+1)=*(DP-1);DP++;
}


void drop()
{
    DP--;
}


void showds()
{
    printf("DS> ");
    CELL *i=DS;
    for (;i<=DP ;i++ )
    {
        printf("%ld ",*i);
    }
    printf("\n");
}


void pick()
{
    CELL k = *DP;
    *DP = *(DP-k);
}


void roll()
{
    CELL k = *DP;
    CELL dk = *(DP-k);
    for(; k>1; k--) {
        *(DP-k) = *(DP-k+1);
    }
    DP--;
    *DP = dk;
}


void invar()
{
    ((Word*)*DP)->num = *(DP-1);
    DP-=2;
}


void outvar() 
{
    *DP = ((Word*)*DP)->num;
}


void equal()
{
    if(*(DP-1) == *DP)
    {
        DP--;
        *DP = 1;
    }
    else{
        DP--;
        *DP = 0;
    }
}


void morethan()
{
    if(*(DP-1) > *DP)
    {
        DP--;
        *DP = 1;
    }
    else{
        DP--;
        *DP = 0;
    }
}


void lessthan()
{
    if(*(DP-1) < *DP)
    {
        DP--;
        *DP = 1;
    }
    else{
        DP--;
        *DP = 0;
    }
}


void iff()
{
    if(*DP==0)
    {
        IP = IP + (CELL)(*(IP+1));
    }
    else
    {
        IP++;
    }
    DP--;
}


void elsee()
{
    IP = IP + (CELL)(*(IP+1));
}


void then()
{
    ;
}


void doo()
{
    if(*(DP-1) <= *DP)
    {
        IP = IP + (CELL)(*(IP+1)); 
        DP--;
        DP--;
    }
    else
    {
        IP++;
        (*DP)++;
        tor();
        tor();
    }
}


void loop() 
{
    IP = IP - (CELL)(*(IP+1)); 
    rto();
    rto();
}


void tor()
{
    RP++;
    *RP=*DP;
    DP--;
}


void rto()
{
    DP++;
    *DP=*RP;
    RP--;
}


void rat()
{
    DP++;
    *DP=*RP;
}


void tot()
{
    TP++;
    *TP=*DP;
    DP--;
}


void tto()
{
    DP++;
    *DP=*TP;
    TP--;
}


void tat()
{
    DP++;
    *DP=*TP;
}


void emit()
{
    putchar((char)(*DP));
    DP--;
}


void myself()
{
    ;
}