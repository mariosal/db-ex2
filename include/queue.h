#ifndef _QUEUE_
#define _QUEUE_

struct node{
    char filename[20];
    struct node * next;
};

struct queue{
    struct node * head;
    struct node * tail;
    int size;
};
int Q_Size(struct queue * Q);
void Q_Init(struct queue * Q);
int Q_Insert(struct queue * Q,char * file);
int Q_Get(struct queue * Q,char ** file);

#endif
