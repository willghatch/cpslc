
all: cpslc

lex.yy.c: cpsl.lex
	flex cpsl.lex

cpslc: main.c lex.yy.c
	gcc --std=c99 -o cpslc main.c
