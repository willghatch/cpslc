#include <stdio.h>
#include <stdlib.h>
#include "symtab.h"
#include "parser.h"

//extern union yylvalUnion yylval;
extern FILE *yyin;
int yylex();
//void yyparse();

int main(int argc, char **argv)
{
    FILE *infile = stdin;
    if (argc > 1)
    {
        infile = fopen(argv[1], "r");
    }
    yyin = infile;

    // Initialize symbol table
    symtabInit();
    ++currscope;

    // Print out some help stuff...
    printf("\nTo use, either specify a file as the first (and only) argument, or pipe the desired\n");
    printf("input to stdin.\n");

    printf("\n\n\n");

    // Parse stuff!
    yyparse();
    scopePrint(currscope);
    --currscope;
    scopePrint(currscope);

    // If it reaches this line without breaking and exiting, we're good.
    printf("File parsed without error.\n");

    

    return 0;
}

