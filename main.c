#include <stdio.h>
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
    
    int lexval;
    do {
        lexval = yylex();
        printf("yylex value: %d\n",lexval);
        switch(lexval) 
        {
            case IDENTSYM: printf("symbol: %s\n", yylval.str_val); break;
            case NUMERICALSYM: printf("number: %i\n", yylval.int_val); break;
            case CHARACTERSYM: printf("character: %c\n", yylval.char_val); break;
            case STRINGSYM: printf("string: %s\n", yylval.str_val); break;
            case FLEX_EOF_SYM: printf("End of input file\n", yylval.str_val); break;

            default : printf("not a symbol with extra data\n");
        }
        
    } while( lexval != 0);

    return 0;
}

