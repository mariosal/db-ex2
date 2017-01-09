#include "record.h"
#include <string.h>

void RecordInitialize(struct Record* record, int id, const char* name,
                      const char* surname, const char* city) {
  record->id = id;
  strncpy(record->name, name,strlen(record->name) - 1);
  strncpy(record->surname, surname,strlen(record->surname) - 1);
  strncpy(record->city, city, strlen(record->city) - 1);
  record->name[strlen(record->name) - 1] = '\0';
  record->surname[strlen(record->surname) - 1] = '\0';
  record->city[strlen(record->city) - 1] = '\0';
}

int RecordPrint(const struct Record* record, FILE* out) {
  return fprintf(out, "%d %s %s %s\n", record->id, record->name,
                 record->surname, record->city);
}
