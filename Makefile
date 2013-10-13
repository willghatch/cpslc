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

parser.h: cpslc.tab.c
cpslc.tab.c: cpslc.y
	bison cpslc.y

cpslc.tab.o: cpslc.tab.c
	$(CC) $(CFLAGS) cpslc.tab.c 

main.o: main.c parser.h
	$(CC) $(CFLAGS) main.c 

symtab.c: symtab.h
symtab.o: symtab.c
	$(CC) $(CFLAGS) symtab.c 

expression.c: expression.h
expression.o: expression.c
	$(CC) $(CFLAGS) expression.c 

cpslc: main.o lex.yy.o cpslc.tab.o symtab.o expression.o
	$(CC) -o cpslc main.o lex.yy.o cpslc.tab.o expression.o symtab.o
