# Mmm... I'll generalize this and remove duplication later... for now it's hard coded.

CC=gcc
BASE_CFLAGS=--std=gnu99 -c
DEBUGFLAGS=-g
RELEASEFLAGS=-O3

CFLAGS=$(BASE_CFLAGS) $(DEBUGFLAGS)

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

slist.c: slist.h
slist.o: slist.c
	$(CC) $(CFLAGS) slist.c 

mipsout.c: mipsout.h
mipsout.o: mipsout.c
	$(CC) $(CFLAGS) mipsout.c 

register.c: register.h
register.o: register.c
	$(CC) $(CFLAGS) register.c 

statement.c: statement.h
statement.o: statement.c
	$(CC) $(CFLAGS) statement.c 

function.c: function.h
function.o: function.c
	$(CC) $(CFLAGS) function.c 

cpslc: main.o lex.yy.o cpslc.tab.o symtab.o expression.o slist.o mipsout.o register.o statement.o function.o
	$(CC) -o cpslc main.o lex.yy.o cpslc.tab.o expression.o symtab.o slist.o mipsout.o register.o statement.o function.o

