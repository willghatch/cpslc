
#include "symtab.h"
#include "expression.h"
#include <stdlib.h>

expr* newNumExpr(int val) {
    expr* e;
    e = malloc(sizeof(expr));
    e->type = int_type;
    e->kind = constant;
    e->int_val = val;
    return e;
}

expr* newCharExpr(char val) {
    expr* e;
    e = malloc(sizeof(expr));
    e->type = char_type;
    e->kind = constant;
    e->char_val = val;
    return e;
}

expr* newStrExpr(char* val) {
    expr* e;
    e = malloc(sizeof(expr));
    e->type = str_type;
    e->kind = constant;
    e->str_val = val;
    return e;
}

