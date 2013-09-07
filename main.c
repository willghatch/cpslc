#include <stdio.h>
#include "symbols.h"
#include "yylval.h"

extern union yylvalUnion yylval;
int yylex();

int main()
{
    while(!feof(stdin)) {
        int lexval = yylex();
        printf("yylex value: %d\n",lexval);
        switch(lexval) 
        {
            case IDENTSYM: printf("symbol: %s\n", yylval.str_val); break;
            case NUMERICALSYM: printf("number: %i\n", yylval.int_val); break;
            case CHARACTERSYM: printf("character: %c\n", yylval.char_val); break;
            case STRINGSYM: printf("string: %s\n", yylval.str_val); break;
            default : printf("not a symbol with extra data\n");
        }
        
    }

    return 0;
}

