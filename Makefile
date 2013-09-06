
all: cpslc

lex.yy.c: cpsl.lex
	flex cpsl.lex

cpslc: main.c lex.yy.c
	gcc -o cpslc main.c
