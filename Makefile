
all: cpslc

clean:
	rm cpslc *.o lex.yy.c

lex.yy.c: cpsl.lex
	flex cpsl.lex

lex.yy.o: lex.yy.c
	gcc --std=c99 lex.yy.c -c

main.o: main.c
	gcc --std=c99 main.c -c 

cpslc: main.o lex.yy.o
	gcc --std=c99 -o cpslc main.o lex.yy.o
