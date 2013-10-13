#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "parser.h"

//extern union yylvalUnion yylval;
extern FILE *yyin;
int yylex();
//void yyparse();

int main(int argc, char **argv)
{
    for(int i = 0; i < argc; ++i) {
        if(!strcmp(argv[i], "--verbose")) {
            verbosity = 1;
        }
    }


    FILE *infile = stdin;
    if (argc > 1)
    {
        infile = fopen(argv[1], "r");
    }
    yyin = infile;

    // Initialize symbol table
    symtabInit();
    pushScope();

    // Print out some help stuff...
    printf("\nTo use, either specify a file as the first (and only) argument, or pipe the desired\n");
    printf("input to stdin.\n");

    printf("\n\n\n");

    // Parse stuff!
    yyparse();

    popScope();
    popScope(); // Incidentally, this call makes the scope stack pointer go out of bounds.  But hey, at this point I don't care.

    // If it reaches this line without breaking and exiting, we're good.
    printf("File parsed without error.\n");

    

    return 0;
}

