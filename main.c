#include <stdio.h>
#include <stdlib.h>

extern union yylvalUnion yylval;
extern FILE *yyin;
int yylex();
void yyparse();

int main(int argc, char **argv)
{
    FILE *infile = stdin;
    if (argc > 1)
    {
        infile = fopen(argv[1], "r");
    }
    yyin = infile;
/*
    printf("CPSL Compiler by William Hatch\nExcept this version just does lexical analysis\n");
    printf("It prints out the symbol number value for each symbol it encounters, and tells what extra \n");
    printf("value (such as identifier name, constant value) that pertains to it.\n");
    printf("To know the name of a symbol instead of the number, please look in symbols.h.  I didn't want\n");
    printf("to make something to do that, because it would be a hassle and isn't actually needed for the\n");
    printf("lexical analysis, and I have other homework to do now.\n");
    printf("\nTo use, either specify a file as the first (and only) argument, or pipe the desired\n");
    printf("input to stdin.\n");
 */   
    yyparse();

    printf("File parsed without error.\n");

    return 0;
}

