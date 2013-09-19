# Mmm... I'll generalize this and remove duplication later... for now it's hard coded.

all: cpslc lextest

lextest: lextest_main.o lex.yy.o
	gcc --std=c99 -o lextest lextest_main.o lex.yy.o

clean:
	rm cpslc *.o lex.yy.c cpslc.tab.c parser.h lextest 

lex.yy.c: cpslc.lex
	flex cpslc.lex

lex.yy.o: lex.yy.c cpslc.tab.c
	gcc --std=c99 lex.yy.c -c

cpslc.tab.c: cpslc.y
	bison cpslc.y

cpslc.tab.o: cpslc.tab.c
	gcc --std=c99 cpslc.tab.c -c

main.o: main.c
	gcc --std=c99 main.c -c 

cpslc: main.o lex.yy.o cpslc.tab.o
	gcc --std=c99 -o cpslc main.o lex.yy.o cpslc.tab.o
