#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

struct QueueNode {
  char value[L_tmpnam];
  struct QueueNode* next;
};

struct Queue {
  size_t len;
  struct QueueNode* front;
  struct QueueNode* back;
};

void Queue_Init(struct Queue** q) {
  *q = malloc(sizeof(**q));
  if (*q == NULL) {
    exit(EXIT_FAILURE);
  }
  (*q)->len = 0;
  (*q)->front = (*q)->back = NULL;
}

void Queue_Reset(struct Queue** q) {
  if (*q != NULL) {
    struct QueueNode* iter = (*q)->front;
    while (iter != NULL) {
      struct QueueNode* tmp = iter;
      iter = iter->next;
      free(tmp);
    }
    free(*q);
    *q = NULL;
  }
}

size_t Queue_Len(const struct Queue* q) {
  return q->len;
}

void Queue_Pop(struct Queue* q, char* value) {
  if (q->len == 0) {
    value = NULL;
    return;
  }
  snprintf(value, sizeof(q->front->value), "%s", q->front->value);
  struct QueueNode* tmp = q->front;
  q->front = q->front->next;
  if (q->front == NULL) {
    q->back = NULL;
  }
  free(tmp);
  --q->len;
}

void Queue_Push(struct Queue* q, const char* value) {
  struct QueueNode* tmp = malloc(sizeof(*tmp));
  if (tmp == NULL) {
    exit(EXIT_FAILURE);
  }
  snprintf(tmp->value, sizeof(tmp->value), "%s", value);
  tmp->next = NULL;
  if (q->front == NULL) {
    q->front = tmp;
  } else {
    q->back->next = tmp;
  }
  q->back = tmp;
  ++q->len;
}
