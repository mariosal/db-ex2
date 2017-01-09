# Exercise configuration

OBJS = build/main.o \
       build/record.o \
       build/sorted.o

BIN = build/sorted.out

# Flags passed to preprocessor, compiler, and linker

CPPFLAGS += -iquoteinclude
CFLAGS += -std=c99 -pedantic-errors
ifeq ($(CC), clang)
	CFLAGS += -Weverything -g
else
	CFLAGS += -Wall -Wextra -g3
endif
LDFLAGS += -Llib -lblock$(shell getconf LONG_BIT)

# Build targets

.PHONY = all clean
all : $(BIN)
clean :
	$(RM) -r build/*

# Exercise builds

build/main.o : src/main.c include/BF.h include/sorted.h include/record.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/record.o : src/record.c include/record.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/sorted.o : src/sorted.c include/sorted.h include/record.h include/BF.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN) : $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)
