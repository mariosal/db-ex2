#ifndef QUEUE_H_
#define QUEUE_H_

#include <stddef.h>

struct Queue;

void Queue_Init(struct Queue** q);
void Queue_Reset(struct Queue** q);
size_t Queue_Len(const struct Queue* q);
void Queue_Pop(struct Queue* q, char* value);
void Queue_Push(struct Queue* q, const char* value);

#endif  // QUEUE_H_
