#include "../include/queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void Q_Init(struct queue * Q){
    
    Q->head = NULL;
    Q->tail = NULL;
    Q->size = 0;
}

int Q_Insert(struct queue * Q,char * file){
    
    struct node * tmp = malloc(sizeof(struct node));
    
    if (tmp == NULL) return -1;
    
    tmp->next == NULL;
    strncpy(tmp->filename,file, sizeof(tmp->filename);
    
    if(Q->size == 0){
       Q->head = tmp;
       Q->tail = tmp;               
       Q->size++;
    }
    
    else if(Q->size > 0 ){
       Q->size++;
       Q->tail->next = tmp;
       Q->tail = tmp 
    } 
    
    return 0;

}
int Q_Get(struct queue * Q,char ** file){
    
    if(Q->size ==0) return -1;
    Q->size--;
    strncpy(&file,Q->head->filename,sizeof(Q->head->filename));

    struct node * tmp;

    tmp = Q->head;
    Q->head = Q->head->next;
    free(tmp);

    return 0 ; 

    
}
    

int Q_Size(struct queue * Q){
    return Q->size;
}
