#ifndef SORTED_H_
#define SORTED_H_

#include "record.h"

int Sorted_CreateFile(const char* filename);
int Sorted_OpenFile(const char* filename);
int Sorted_CloseFile(int fd);
int Sorted_InsertEntry(int fd, struct Record record);
int Sorted_SortFile(const char* filename, int field_no);
int Sorted_checkSortedFile(const char* filename, int fieldno);
void Sorted_GetAllEntries(int fd, const int* fiedno, const void* value);

#endif  // SORTED_H_
