CC = gcc
CFLAGS = -Wall -g
SUBOBJS = bitmap.o hash.o list.o list_expand.o hash_expand.o parse.o shell.o instruction.o
OBJS = main.o $(SUBOBJS)
TARGET = testlib

.SUFFIXES : .c .o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CLFAGS) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

bitmap.o : bitmap.c bitmap.h limits.h round.h stdio.c
hash.o : hash.c hash.h
list.o : list.c list.h
list_expand.o : list_expand.c list_expand.h
hash_expand.o : hash_expand.c hash_expand.h
parse.o : parse.c parse.h
shell.o : shell.c shell.h 
instruction.o : instruction.c instruction.h
main.o : main.c main.h $(SUBOBJS)
