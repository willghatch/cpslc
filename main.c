#include <stdio.h>

int yylex();

int main()
{
    printf("%d\n", yylex());

    return 0;
}

