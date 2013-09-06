#include "lex.yy.c"
#include <stdio>

int main()
{
    printf(yylex());

    return 0;
}

