#include "sorted.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "BF.h"

struct Sorted_Info {
  bool heap;
  int num_entries;
};

static int Sorted_MaxEntries(void) {
  return BLOCK_SIZE / sizeof(struct Record);
}

static void Sorted_SetHeap(void* block, bool heap) {
  block = (unsigned char*)block + offsetof(struct Sorted_Info, heap);
  memcpy(block, &heap, sizeof(heap));
}

static bool Sorted_Heap(void* block) {
  bool heap;
  block = (unsigned char*)block + offsetof(struct Sorted_Info, heap);
  memcpy(&heap, block, sizeof(heap));
  return heap;
}

static void Sorted_SetNumEntries(void* block, int num_entries) {
  block = (unsigned char*)block + offsetof(struct Sorted_Info, num_entries);
  memcpy(block, &num_entries, sizeof(num_entries));
}

static int Sorted_NumEntries(void* block) {
  int num_entries;
  block = (unsigned char*)block + offsetof(struct Sorted_Info, num_entries);
  memcpy(&num_entries, block, sizeof(num_entries));
  return num_entries;
}

int Sorted_CreateFile(const char* filename) {
  if (BF_CreateFile(filename) == -1) {
    BF_PrintError("Error creating file");
    return -1;
  }
  int fd = BF_OpenFile(filename);
  if (fd == -1) {
    BF_PrintError("Error opening file");
    return -1;
  }

  if (BF_AllocateBlock(fd) == -1) {
    BF_PrintError("Error allocating block");
    return -1;
  }
  void* block;
  if (BF_ReadBlock(fd, 0, &block) == -1) {
    BF_PrintError("Error reading block");
    return -1;
  }
  Sorted_SetHeap(block, true);
  Sorted_SetNumEntries(block, 0);
  if (BF_WriteBlock(fd, 0) == -1) {
    BF_PrintError("Error writing block");
    return -1;
  }

  if (BF_CloseFile(fd) == -1) {
    BF_PrintError("Error closing file");
    return -1;
  }
  return 0;
}

int Sorted_OpenFile(const char* filename) {
  int fd = BF_OpenFile(filename);
  if (fd == -1) {
    BF_PrintError("Error opening file");
    return -1;
  }
  if (BF_GetBlockCounter(fd) == 0) {
    return -1;
  }

  void* block;
  if (BF_ReadBlock(fd, 0, &block) == -1) {
    BF_PrintError("Error reading block");
    return -1;
  }
  if (!Sorted_Heap(block) || Sorted_NumEntries(block) < 0) {
    return -1;
  }
  int num_blocks = 1 + Sorted_NumEntries(block) / Sorted_MaxEntries();
  if (num_blocks != BF_GetBlockCounter(fd)) {
    return -1;
  }

  return fd;
}

int Sorted_CloseFile(int fd) {
  if (BF_CloseFile(fd) == -1) {
    BF_PrintError("Error closing file");
    return -1;
  }
  return 0;
}

int Sorted_InsertEntry(int fd, struct Record Record) {
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
