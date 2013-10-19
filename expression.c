// expressions.c

#include "symtab.h"
#include "expression.h"
#include <stdlib.h>

expr* newNumExpr(int val) {
    expr* e;
    e = malloc(sizeof(expr));
    e->type = int_type;
    e->kind = constant_expr;
    e->int_val = val;
    return e;
}

expr* newCharExpr(char val) {
    expr* e;
    e = malloc(sizeof(expr));
    e->type = char_type;
    e->kind = constant_expr;
    e->char_val = val;
    return e;
}

expr* newStrExpr(char* val) {
    expr* e;
    e = malloc(sizeof(expr));
    e->type = str_type;
    e->kind = constant_expr;
    e->str_val = val;
    return e;
}

expr* newBoolExpr(bool val) {
    expr* e;
    e = malloc(sizeof(expr));
    e->type = bool_type;
    e->kind = constant_expr;
    e->str_val = val;
    return e;
}

expr* newRegExpr(int reg, TYPE* type) {
    expr* e;
    e = malloc(sizeof(expr));
    e->type = type;
    e->kind = registerVal;
    e->reg_number = reg;
    return e;
}


char* strdup_mips_escaped(char* str) {
// strdup, only it escapes \n and \t characters that spim handles
    char* newstr;
    int len = strlen(str);
    int numEsc = 0;
    for(int i = 0; i < len; ++i) {
        switch(str[i]) {
            case '\n':
            case '\t':
            // It seems spim only handles these two escape sequences.
                ++numEsc;
                break;
            default:
                break;
        }
    }
    newstr = malloc((len + numEsc) * sizeof(char));

    int nsi = 0; // new string i
    for(int i = 0; i < len; ++i) {
        switch(str[i]) {
            case '\n':
                newstr[nsi++] = '\\';
                newstr[nsi++] = 'n';
            case '\t':
                newstr[nsi++] = '\\';
                newstr[nsi++] = 't';
            default:
                newstr[nsi++] = str[i];

        }
    }
    return newstr;
}

