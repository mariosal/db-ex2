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
  (*q)->front = (*q)->back = malloc(sizeof(*(*q)->back));
  (*q)->front->next = NULL;
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
  snprintf(value, sizeof(q->front->next->value), "%s", q->front->next->value);
  struct QueueNode* tmp = q->front->next->next;
  free(q->front->next);
  q->front->next = tmp;
  --q->len;
}

void Queue_Push(struct Queue* q, const char* value) {
  q->back->next = malloc(sizeof(*q->back->next));
  if (q->back->next == NULL) {
    exit(EXIT_FAILURE);
  }
  q->back = q->back->next;
  snprintf(q->back->value, sizeof(q->back->value), "%s", value);
  q->back->next = NULL;
  ++q->len;
}
