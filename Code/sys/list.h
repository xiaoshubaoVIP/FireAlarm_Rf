#ifndef __LIST_H__
#define __LIST_H__

#ifndef DATA_TYPE
#define DATA_TYPE void*
#endif

typedef DATA_TYPE data;

typedef struct node
{
    data d;
    struct node *next;
} node;

typedef node* list;

#endif
