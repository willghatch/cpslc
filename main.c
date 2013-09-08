#include <stdio.h>
#include <stdlib.h>
#include "symbols.h"
#include "yylval.h"

extern union yylvalUnion yylval;
extern FILE *yyin;
int yylex();

int main(int argc, char **argv)
{
    FILE *infile = stdin;
    if (argc > 1)
    {
        infile = fopen(argv[1], "r");
    }
    yyin = infile;

    printf("CPSL Compiler by William Hatch\nExcept this version just does lexical analysis\n");
    printf("It prints out the symbol number value for each symbol it encounters, and tells what extra \n");
    printf("value (such as identifier name, constant value) that pertains to it.\n");
    printf("To know the name of a symbol instead of the number, please look in symbols.h.  I didn't want\n");
    printf("to make something to do that, because it would be a hassle and isn't actually needed for the\n");
    printf("lexical analysis, and I have other homework to do now.\n");
    printf("\nTo use, either specify a file as the first (and only) argument, or pipe the desired\n");
    printf("input to stdin.\n");
    
    int lexval;
    do {
        lexval = yylex();
        printf("Symbol number found: %d\n",lexval);
        switch(lexval) 
        {
            case IDENTSYM: printf("identifier: %s\n", yylval.str_val); break;
            case NUMERICALSYM: printf("number: %i\n", yylval.int_val); break;
            case CHARACTERSYM: printf("character: %c\n", yylval.char_val); break;
            case STRINGSYM: printf("string: %s\n", yylval.str_val); break;
            case FLEX_EOF_SYM: printf("End of input file\n", yylval.str_val); break;

            case ERRORSYM: printf("Error.  Aborting.\n"); exit(1);

            default : printf("not a symbol with extra data\n");
        }
        
    } while( lexval != 0);

    return 0;
}

