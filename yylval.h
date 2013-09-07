#ifndef YYLVAL_H
#define YYLVAL_H

union yylvalUnion {
    int int_val;
    char char_val;
    char *str_val;
};


#endif
