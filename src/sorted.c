#include "sorted.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "BF.h"
#include "queue.h"

struct Sorted_Info {
  bool heap;
  int num_entries;
};

static int Ceil(int x, int y) {
  if (x == 0) {
    return 0;
  }
  return 1 + (x - 1) / y;
}

static int Min(int a, int b) {
  if (a > b) {
    return b;
  }
  return a;
}

static int Compare(const struct Record* a, const struct Record* b,
                   int fieldno) {
  if (a == NULL) {
    return 1;
  }
  if (b == NULL) {
    return -1;
  }
  switch (fieldno) {
    case 0: {
      return a->id - b->id;
    }
    case 1: {
      return strcmp(a->name, b->name);
    }
    case 2: {
      return strcmp(a->surname, b->surname);
    }
    case 3: {
      return strcmp(a->city, b->city);
    }
    default: {
      return 0;
    }
  }
}

static int CompareValue(const struct Record* a, const void* value, int fieldno) {
  if (a == NULL) {
    return 1;
  }
  if (value == NULL) {
    return -1;
  }
  switch (fieldno) {
    case 0: {
      return a->id - *(const int*)value;
    }
    case 1: {
      return strcmp(a->name, value);
    }
    case 2: {
      return strcmp(a->surname, value);
    }
    case 3: {
      return strcmp(a->city, value);
    }
    default: {
      return 0;
    }
  }
}

static void Quicksort(struct Record* a, int len, int fieldno) {
  if (len <= 1) {
    return;
  }

  struct Record pivot = a[len / 2];
  int i, j;
  for (i = 0, j = len - 1; ; ++i, --j) {
    while (Compare(&a[i], &pivot, fieldno) < 0) {
      ++i;
    }
    while (Compare(&a[j], &pivot, fieldno) > 0) {
      --j;
    }
    if (i >= j) {
      break;
    }
    const struct Record tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
  }

  Quicksort(a, i, fieldno);
  Quicksort(a + i, len - i, fieldno);
}

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

static int Sorted_SetEntry(int fd, int offset, const struct Record* record) {
  void* block;
  int block_id = Ceil(offset + 1, Sorted_MaxEntries());
  if (BF_ReadBlock(fd, block_id, &block) < 0) {
    BF_PrintError("Error reading block");
    return -1;
  }

  int block_offset = offset - (block_id - 1) * Sorted_MaxEntries();
  block = (unsigned char*)block + sizeof(*record) * block_offset;
  memcpy(block, record, sizeof(*record));
  return block_id;
}

static struct Record* Sorted_Entry(int fd, int offset) {
  void* block;
  int block_id = Ceil(offset + 1, Sorted_MaxEntries());
  if (BF_ReadBlock(fd, block_id, &block) < 0) {
    BF_PrintError("Error reading block");
    return NULL;
  }

  struct Record* record = malloc(sizeof(*record));
  if (record == NULL) {
    exit(EXIT_FAILURE);
  }
  int block_offset = offset - (block_id - 1) * Sorted_MaxEntries();
  block = (unsigned char*)block + sizeof(*record) * block_offset;
  memcpy(record, block, sizeof(*record));
  return record;
}

static int Sorted_Split(int fd_orig, int fieldno, struct Queue* q) {
  void* zero_orig;
  if (BF_ReadBlock(fd_orig, 0, &zero_orig) < 0) {
    BF_PrintError("Error reading block");
    return -1;
  }

  char buf[L_tmpnam];
  int num_entries_orig = Sorted_NumEntries(zero_orig);
  for (int i = 1; i < BF_GetBlockCounter(fd_orig); ++i) {
    void* block_orig;
    if (BF_ReadBlock(fd_orig, i, &block_orig) < 0) {
      BF_PrintError("Error reading block");
      return -1;
    }

    tmpnam(buf);
    if (Sorted_CreateFile(buf) < 0) {
      return -1;
    }
    Queue_Push(q, buf);

    int fd_new = Sorted_OpenFile(buf);
    if (fd_new < 0) {
      return -1;
    }

    void* zero_new;
    if (BF_ReadBlock(fd_new, 0, &zero_new) < 0) {
      BF_PrintError("Error reading block");
      return -1;
    }
    int num_entries_new = Min(Sorted_MaxEntries(), num_entries_orig);
    Sorted_SetNumEntries(zero_new, num_entries_new);
    if (BF_WriteBlock(fd_new, 0) < 0) {
      BF_PrintError("Error writing block");
      return -1;
    }

    if (BF_AllocateBlock(fd_new) < 0) {
      BF_PrintError("Error allocating block");
      return -1;
    }
    void* one_new;
    if (BF_ReadBlock(fd_new, 1, &one_new) < 0) {
      BF_PrintError("Error reading block");
      return -1;
    }
    struct Record* entries = malloc(sizeof(*entries) * num_entries_new);
    if (entries == NULL) {
      exit(EXIT_FAILURE);
    }
    for (int j = 0; j < num_entries_new; ++j) {
      memcpy(&entries[j], block_orig, sizeof(*entries));
      block_orig = (unsigned char*)block_orig + sizeof(*entries);
    }

    Quicksort(entries, Sorted_NumEntries(zero_new), fieldno);
    for (int j = 0; j < num_entries_new; ++j) {
      memcpy(one_new, &entries[j], sizeof(*entries));
      one_new = (unsigned char*)one_new + sizeof(*entries);
    }
    free(entries);
    if (BF_WriteBlock(fd_new, 1) < 0) {
      BF_PrintError("Error writing block");
      return -1;
    }

    if (Sorted_CloseFile(fd_new) < 0) {
      return -1;
    }
    num_entries_orig -= Sorted_MaxEntries();
  }
  return 0;
}

static int Sorted_Merge(const char* f1, const char* f2, int fieldno,
                        struct Queue* q) {
  int fd1 = Sorted_OpenFile(f1);
  if (fd1 < 0) {
    return -1;
  }
  int fd2 = Sorted_OpenFile(f2);
  if (fd2 < 0) {
    return -1;
  }

  char buf[L_tmpnam];
  tmpnam(buf);
  if (Sorted_CreateFile(buf) < 0) {
    return -1;
  }
  Queue_Push(q, buf);

  int fd3 = Sorted_OpenFile(buf);
  if (fd3 < 0) {
    return -1;
  }

  void* zero1;
  if (BF_ReadBlock(fd1, 0, &zero1) < 0) {
    BF_PrintError("Error reading block");
    return -1;
  }
  int max1 = Sorted_NumEntries(zero1);

  void* zero2;
  if (BF_ReadBlock(fd2, 0, &zero2) < 0) {
    BF_PrintError("Error reading block");
    return -1;
  }
  int max2 = Sorted_NumEntries(zero2);

  void* zero3;
  if (BF_ReadBlock(fd3, 0, &zero3) < 0) {
    BF_PrintError("Error reading block");
    return -1;
  }
  Sorted_SetNumEntries(zero3, max1 + max2);
  if (BF_WriteBlock(fd3, 0) < 0) {
    BF_PrintError("Error writing block");
    return -1;
  }

  int i = 0;
  int j = 0;
  for (int k = 0; k < max1 + max2; ++k) {
    if (k % Sorted_MaxEntries() == 0 && BF_AllocateBlock(fd3) < 0) {
      BF_PrintError("Error allocating block");
      return -1;
    }

    struct Record* left = NULL;
    if (i < max1) {
      left = Sorted_Entry(fd1, i);
      if (left == NULL) {
        return -1;
      }
    }
    struct Record* right = NULL;
    if (j < max2) {
      right = Sorted_Entry(fd2, j);
      if (right == NULL) {
        return -1;
      }
    }

    int block_id;
    if (i < max1 && Compare(left, right, fieldno) <= 0) {
      if (left == NULL) {
        return -1;
      }
      block_id = Sorted_SetEntry(fd3, k, left);
      ++i;
    } else {
      if (right == NULL) {
        return -1;
      }
      block_id = Sorted_SetEntry(fd3, k, right);
      ++j;
    }
    if (block_id < 0) {
      return -1;
    }
    if (BF_WriteBlock(fd3, block_id) < 0) {
      BF_PrintError("Error writing block");
      return -1;
    }
    free(left);
    free(right);
  }

  if (Sorted_CloseFile(fd1) < 0) {
    return -1;
  }
  if (Sorted_CloseFile(fd2) < 0) {
    return -1;
  }
  if (Sorted_CloseFile(fd3) < 0) {
    return -1;
  }
  return 0;
}

static int Sorted_Move(const char* f1, const char* f2) {
  int fd1 = Sorted_OpenFile(f1);
  if (fd1 < 0) {
    return -1;
  }

  if (Sorted_CreateFile(f2) < 0) {
    return -1;
  }
  int fd2 = Sorted_OpenFile(f2);
  if (fd2 < 0) {
    return -1;
  }

  for (int i = 0; i < BF_GetBlockCounter(fd1); ++i) {
    void* block1;
    if (BF_ReadBlock(fd1, i, &block1) < 0) {
      BF_PrintError("Error reading block");
      return -1;
    }

    if (i > 0 && BF_AllocateBlock(fd2) < 0) {
      BF_PrintError("Error allocating block");
      return -1;
    }
    void* block2;
    if (BF_ReadBlock(fd2, i, &block2) < 0) {
      BF_PrintError("Error reading block");
      return -1;
    }

    memcpy(block2, block1, BLOCK_SIZE);
    if (BF_WriteBlock(fd2, i) < 0) {
      BF_PrintError("Error writing block");
      return -1;
    }
  }
  if (Sorted_CloseFile(fd1) < 0) {
    return -1;
  }
  remove(f1);
  if (Sorted_CloseFile(fd2) < 0) {
    return -1;
  }
  return 0;
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

  int num_blocks = 1 + Ceil(Sorted_NumEntries(zero), Sorted_MaxEntries());
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
  int num_entries = Sorted_NumEntries(zero);
  Sorted_SetNumEntries(zero, num_entries + 1);
  if (BF_WriteBlock(fd, 0) < 0) {
    BF_PrintError("Error writing block");
    return -1;
  }
  if (num_entries % Sorted_MaxEntries() == 0) {
    if (BF_AllocateBlock(fd) < 0) {
      BF_PrintError("Error allocating block");
      return -1;
    }
  }

  int block_id = Sorted_SetEntry(fd, num_entries, &record);
  if (block_id < 0) {
    return -1;
  }
  if (BF_WriteBlock(fd, block_id) < 0) {
    BF_PrintError("Error writing block");
    return -1;
  }
  return 0;
}

int Sorted_SortFile(const char* filename, int fieldno) {
  int fd_orig = Sorted_OpenFile(filename);
  if (fd_orig < 0) {
    return -1;
  }
  void* zero_orig;
  if (BF_ReadBlock(fd_orig, 0, &zero_orig) < 0) {
    BF_PrintError("Error reading block");
    return -1;
  }
  if (Sorted_NumEntries(zero_orig) <= 1) {
    return 0;
  }

  struct Queue* q;
  Queue_Init(&q);
  if (Sorted_Split(fd_orig, fieldno, q) < 0) {
    return -1;
  }

  if (Sorted_CloseFile(fd_orig) < 0) {
    return -1;
  }

  char buf1[L_tmpnam], buf2[L_tmpnam];
  while (Queue_Len(q) > 1) {
    Queue_Pop(q, buf1);
    Queue_Pop(q, buf2);
    if (Sorted_Merge(buf1, buf2, fieldno, q) < 0) {
      return -1;
    }
    remove(buf1);
    remove(buf2);
  }

  Queue_Pop(q, buf1);
  char sorted_filename[512];
  snprintf(sorted_filename, sizeof(sorted_filename), "%sSorted%d", filename,
           fieldno);
  Queue_Reset(&q);

  if (Sorted_Move(buf1, sorted_filename) < 0) {
    return -1;
  }
  return 0;
}

int Sorted_checkSortedFile(const char* filename, int fieldno) {
  int fd = Sorted_OpenFile(filename);
  if (fd < 0) {
    return -1;
  }
  void* zero;
  if (BF_ReadBlock(fd, 0, &zero) < 0) {
    BF_PrintError("Error reading block");
    return -1;
  }
  int num_entries = Sorted_NumEntries(zero);

  int ret = 0;
  for (int i = 0; i < num_entries - 1; ++i) {
    struct Record* cur = Sorted_Entry(fd, i);
    if (cur == NULL) {
      return -1;
    }
    struct Record* next = Sorted_Entry(fd, i + 1);
    if (next == NULL) {
      return -1;
    }
    if (Compare(cur, next, fieldno) > 0) {
      free(cur);
      free(next);
      ret = -1;
      break;
    }
    free(cur);
    free(next);
  }
  if (Sorted_CloseFile(fd) < 0) {
    return -1;
  }
  return ret;
}

void Sorted_GetAllEntries(int fd, const int* fieldno, const void* value) {
  void* zero;
  if (BF_ReadBlock(fd, 0, &zero) < 0) {
    BF_PrintError("Error reading block");
    return;
  }
  int num_entries = Sorted_NumEntries(zero);

  if (value == NULL) {
    for (int i = 0; i < num_entries; ++i) {
      struct Record* cur = Sorted_Entry(fd, i);
      RecordPrint(cur, stdout);
      free(cur);
    }
    printf("Read %d blocks\n", 1 + Ceil(num_entries, Sorted_MaxEntries()));
  } else {
    bool* read_blocks = malloc(sizeof(*read_blocks) * BF_GetBlockCounter(fd));
    if (read_blocks == NULL) {
      exit(EXIT_FAILURE);
    }
    memset(read_blocks, 0, sizeof(*read_blocks) * BF_GetBlockCounter(fd));
    read_blocks[0] = true;

    int lo = 0;
    int hi = num_entries - 1;
    while (lo < hi) {
      int mi = lo + (hi - lo) / 2;
      struct Record* r = Sorted_Entry(fd, mi);
      read_blocks[Ceil(mi + 1, Sorted_MaxEntries())] = true;
      if (CompareValue(r, value, *fieldno) >= 0) {
        hi = mi;
      } else {
        lo = mi + 1;
      }
      free(r);
    }
    for (; lo < num_entries; ++lo) {
      struct Record* r = Sorted_Entry(fd, lo);
      read_blocks[Ceil(lo + 1, Sorted_MaxEntries())] = true;
      if (CompareValue(r, value, *fieldno) != 0) {
        free(r);
        break;
      }
      RecordPrint(r, stdout);
      free(r);
    }

    int num_blocks = 0;
    for (int i = 0; i < BF_GetBlockCounter(fd); ++i) {
      num_blocks += read_blocks[i];
    }
    free(read_blocks);
    printf("Read %d blocks\n", num_blocks);
  }
}
