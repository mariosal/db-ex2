#include "sorted.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "BF.h"

struct Sorted_Info {
  bool heap;
  int num_entries;
};

static int Sorted_MaxEntries(void) {
  return BLOCK_SIZE / sizeof(struct Record);
}

static void Sorted_SetHeap(void* zero, bool heap) {
  zero = (unsigned char*)zero + offsetof(struct Sorted_Info, heap);
  memcpy(zero, &heap, sizeof(heap));
}

static bool Sorted_Heap(const void* zero) {
  return ((const struct Sorted_Info*)zero)->heap;
}

static void Sorted_SetNumEntries(void* zero, int num_entries) {
  zero = (unsigned char*)zero + offsetof(struct Sorted_Info, num_entries);
  memcpy(zero, &num_entries, sizeof(num_entries));
}

static int Sorted_NumEntries(const void* zero) {
  return ((const struct Sorted_Info*)zero)->num_entries;
}

static void Sorted_SetEntry(void* block, int offset, const struct Record* record) {
  block = (unsigned char*)block + sizeof(*record) * offset;
  memcpy(block, record, sizeof(*record));
}

static struct Record* Sorted_Entry(const void* block, int offset) {
  struct Record* record = malloc(sizeof(*record));
  block = (const unsigned char*)block + sizeof(*record) * offset;
  memcpy(record, block, sizeof(*record));
  return record;
}

int Sorted_CreateFile(const char* filename) {
  if (BF_CreateFile(filename) < 0) {
    BF_PrintError("Error creating file");
    return -1;
  }
  int fd = BF_OpenFile(filename);
  if (fd < 0) {
    BF_PrintError("Error opening file");
    return -1;
  }

  if (BF_AllocateBlock(fd) < 0) {
    BF_PrintError("Error allocating block");
    return -1;
  }
  void* zero;
  if (BF_ReadBlock(fd, 0, &zero) < 0) {
    BF_PrintError("Error reading block");
    return -1;
  }
  Sorted_SetHeap(zero, true);
  Sorted_SetNumEntries(zero, 0);
  if (BF_WriteBlock(fd, 0) < 0) {
    BF_PrintError("Error writing block");
    return -1;
  }

  if (BF_CloseFile(fd) < 0) {
    BF_PrintError("Error closing file");
    return -1;
  }
  return 0;
}

int Sorted_OpenFile(const char* filename) {
  int fd = BF_OpenFile(filename);
  if (fd < 0) {
    BF_PrintError("Error opening file");
    return -1;
  }
  if (BF_GetBlockCounter(fd) == 0) {
    return -1;
  }

  void* zero;
  if (BF_ReadBlock(fd, 0, &zero) < 0) {
    BF_PrintError("Error reading block");
    return -1;
  }
  if (!Sorted_Heap(zero) || Sorted_NumEntries(zero) < 0) {
    return -1;
  }
  int num_blocks = 1 + Sorted_NumEntries(zero) / Sorted_MaxEntries();
  if (num_blocks != BF_GetBlockCounter(fd)) {
    return -1;
  }

  return fd;
}

int Sorted_CloseFile(int fd) {
  if (BF_CloseFile(fd) < 0) {
    BF_PrintError("Error closing file");
    return -1;
  }
  return 0;
}

int Sorted_InsertEntry(int fd, struct Record record) {
  void* zero;
  if (BF_ReadBlock(fd, 0, &zero) < 0) {
    BF_PrintError("Error reading block");
    return -1;
  }
  if (Sorted_NumEntries(zero) % Sorted_MaxEntries() == 0) {
    if (BF_AllocateBlock(fd) < 0) {
      BF_PrintError("Error allocating block");
      return -1;
    }
  }

  void* back;
  int back_id = BF_GetBlockCounter(fd) - 1;
  if (BF_ReadBlock(fd, back_id, &back) < 0) {
    BF_PrintError("Error reading block");
    return -1;
  }

  int offset = Sorted_NumEntries(zero) - (back_id - 1) * Sorted_MaxEntries();
  Sorted_SetEntry(back, offset, &record);
  if (BF_WriteBlock(fd, back_id) < 0) {
    BF_PrintError("Error writing block");
    return -1;
  }

  Sorted_SetNumEntries(zero, Sorted_NumEntries(zero) + 1);
  if (BF_WriteBlock(fd, 0) < 0) {
    BF_PrintError("Error writing block");
    return -1;
  }
  return 0;
}

int Sorted_SortFile(const char* filename, int fieldno) {
  return 0;
}

int Sorted_checkSortedFile(const char* filename, int fieldno) {
  return 0;
}

void Sorted_GetAllEntries(int fd, const int* fieldno, const void* value) {
}
