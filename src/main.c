#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "BF.h"
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

int main(int argc, char** argv) {
  BF_Init();

  int field_num = 0;
  bool create = true;
  for (int i = 1; i < argc; ++i) {
    if (Equals(argv[i], "f") || Equals(argv[i], "field") ||
        Equals(argv[i], "fieldno")) {
      ++i;
      sscanf(argv[i], "%d", &field_num);
    } else if (Equals(argv[i], "nc") || Equals(argv[i], "nocreate")) {
      create = false;
    }
  }

  if (create) {
  }

  struct Record record;
  memset(&record, 0, sizeof(struct Record));
  while (scanf("%d,\"%14[^\"]\",\"%19[^\"]\",\"%24[^\"]\"", &record.id,
      record.name, record.surname, record.city) == 4) {
  }
  return 0;
}
