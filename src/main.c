#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "BF.h"
#include "size_types.h"
#include "sorted.h"

static bool Equals(const char* a, const char* b) {
  if (a[0] == '-') {
    return Equals(a + 1, b);
  }
  if (b[0] == '-') {
    return Equals(a, b + 1);
  }
  if (tolower(a[0]) == tolower(b[0])) {
    return a[0] == '\0' || Equals(a + 1, b + 1);
  }
  return false;
}

static void* ArgValue(int argc, char** argv, int fieldno) {
  void* value = NULL;
  for (int i = 1; i < argc; ++i) {
    if (Equals(argv[i], "value")) {
      ++i;
      char fmt[8];
      size_t size = 1;
      if (fieldno == 0) {
        size = sizeof(int);
        snprintf(fmt, sizeof(fmt), "%%d");
      } else {
        if (fieldno == 1) {
          size = sizeof(((struct Record*)NULL)->name);
        } else if (fieldno == 2) {
          size = sizeof(((struct Record*)NULL)->surname);
        } else if (fieldno == 3) {
          size = sizeof(((struct Record*)NULL)->city);
        }
        snprintf(fmt, sizeof(fmt), "%%%" PRIuS "s", size - 1);
      }
      value = malloc(size);
      sscanf(argv[i], fmt, value);
    }
  }
  return value;
}

static void InsertEntries(int fd, const char* filename) {
  struct Record record;
  memset(&record, 0, sizeof(record));
  char fmt[64];
  snprintf(fmt, sizeof(fmt), "%%d,\"%%%" PRIuS "[^\"]\",\"%%%" PRIuS "[^\"]\","
           "\"%%%" PRIuS "[^\"]\"", sizeof(record.name) - 1,
           sizeof(record.surname) - 1, sizeof(record.city) - 1);
  while (scanf(fmt, &record.id, record.name, record.surname,
               record.city) == 4) {
    if (Sorted_InsertEntry(fd, record) < 0) {
      fprintf(stderr, "Error inserting {%d, %s, %s, %s} into file %s\n",
              record.id, record.name, record.surname, record.city, filename);
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char** argv) {
  BF_Init();

  int fieldno = 0;
  char filename[512];
  snprintf(filename, sizeof(filename), "build/heap_file");
  for (int i = 1; i < argc; ++i) {
    if (Equals(argv[i], "fieldno")) {
      ++i;
      sscanf(argv[i], "%d", &fieldno);
    } else if (Equals(argv[i], "filename")) {
      ++i;
      sscanf(argv[i], "%s", filename);
    }
  }
  void* value = ArgValue(argc, argv, fieldno);

  if (Sorted_CreateFile(filename) < 0) {
    fprintf(stderr, "Error creating file %s\n", filename);
    return EXIT_FAILURE;
  }

  int fd = Sorted_OpenFile(filename);
  if (fd < 0) {
    fprintf(stderr, "Error opening file %s\n", filename);
    return EXIT_FAILURE;
  }

  InsertEntries(fd, filename);

  if (Sorted_CloseFile(fd) < 0) {
    fprintf(stderr, "Error closing file %s\n", filename);
    return EXIT_FAILURE;
  }

  if (Sorted_SortFile(filename, fieldno) < 0) {
    fprintf(stderr, "Error sorting file %s by fieldno %d\n", filename, fieldno);
    return EXIT_FAILURE;
  }

  char sorted_filename[128];
  snprintf(sorted_filename, sizeof(sorted_filename), "%sSorted%d", filename,
           fieldno);
  if (Sorted_checkSortedFile(sorted_filename, 0) < 0) {
    printf("File %s is not sorted by fieldno %d\n", sorted_filename, fieldno);
  } else {
    printf("File %s is sorted by fieldno %d\n", sorted_filename, fieldno);
  }

  fd = Sorted_OpenFile(sorted_filename);
  if (fd < 0) {
    fprintf(stderr, "Error opening file %s\n", sorted_filename);
    return EXIT_FAILURE;
  }

  Sorted_GetAllEntries(fd, &fieldno, value);
  free(value);

  if (Sorted_CloseFile(fd) < 0) {
    fprintf(stderr, "Error closing file %s\n", sorted_filename);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
