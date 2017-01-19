#ifndef SORTED_H_
#define SORTED_H_

#include "record.h"

int max_records_in_block(void);
int Sorted_IsertEntry(int fileDesc,Record record);
void Sorted_Sort_File(char * fileName, int fieldNo);
void Sorted_checkSortedFile(char * filename, int fieldNo);
void Sorted_GetAllEntries(int fileDesc,int * fieldNo,void * value);
int Sorted_CreateFile(char * filename );
int Sorted_OpenFile(char * filename);
int Sorted_CloseFile(int fileDesc);
#endif  // SORTED_H_
