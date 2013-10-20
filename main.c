#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "parser.h"
#include "register.h"
#include "mipsout.h"

//extern union yylvalUnion yylval;
extern FILE *yyin;
int yylex();
void yyparse();

int main(int argc, char **argv)
{

    FILE *infile = stdin;

    for(int i = 1; i < argc; ++i) {
        if(!strcmp(argv[i], "--verbose")) {
            verbosity = 1;
        }
        else {
            // I'm just assuming this is the file name if it's not the verbose flag
            infile = fopen(argv[1], "r");
        }
    }

    yyin = infile;

    // Initialize stuff
    init_registerState();
    mips_init();
    symtabInit();
    pushScope();

    // Print out some help stuff...
    printf("\nTo use, either specify a file as the first (and only) argument, or pipe the desired\n");
    printf("input to stdin.\n");
    printf("If you want, add --verbose for verbosity.  It's the only option.\n");

    printf("\n\n\n");

    // Parse stuff!
    yyparse();

    popScope();
    popScope(); // Incidentally, this call makes the scope stack pointer go out of bounds.  But hey, at this point I don't care.

    // If it reaches this line without breaking and exiting, we're good.
    printf("File parsed without error.\n");

    m_write_file("output.asm");
    

    return 0;
}

