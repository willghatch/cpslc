# Mmm... I'll generalize this and remove duplication later... for now it's hard coded.

CC=gcc
CFLAGS=--std=gnu99 -c

all: cpslc lextest

lextest: lextest_main.o lex.yy.o
	$(CC) -o lextest lextest_main.o lex.yy.o

clean:
	rm cpslc *.o lex.yy.c cpslc.tab.c parser.h lextest 

lex.yy.c: cpslc.lex
	flex cpslc.lex

lex.yy.o: lex.yy.c cpslc.tab.c
	$(CC) $(CFLAGS) lex.yy.c 

cpslc.tab.c: cpslc.y
	bison cpslc.y

cpslc.tab.o: cpslc.tab.c
	$(CC) $(CFLAGS) cpslc.tab.c 

main.o: main.c
	$(CC) $(CFLAGS) main.c 

symtab.c: symtab.h
symtab.o: symtab.c
	$(CC) $(CFLAGS) symtab.c 

cpslc: main.o lex.yy.o cpslc.tab.o
	$(CC) -o cpslc main.o lex.yy.o cpslc.tab.o
