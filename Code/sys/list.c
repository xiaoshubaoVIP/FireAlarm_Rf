#include "list.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

void List_Add(list l, node* n)
{
    l->next = n;
    l = n;
}

void List_Del(list l, node* n)
{
    node* last_note = l;

    for (; l != NULL; l = l->next) {
        if (n == l) { // TODO 要删除的是头部的情况
            last_note->next = n->next;
            return;
        }

        last_note = l;
    }
}

void List_Foreach(list l, void (*handler)(node*))
{
    for (; l != NULL; l = l->next)
        handler(l->d);
}


