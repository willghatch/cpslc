
#ifndef EXPRESSION_H_
#define EXPRESSION_H_

// So I need something to hold values for expressions...

#include"symtab.h"

typedef enum expr_kind_enum expr_kind;
enum expr_kind_enum {
    constant_expr, lValue, procedureCall, operation, parenthesized
};

typedef struct expression_struct expr;
struct expression_struct {
    TYPE* type;
    expr_kind kind;
    int int_val;
    char char_val;
    char* str_val;
    // I should have some way to hold the value of generic types... but when
    // they're not constant I'll have to have part of the parse tree or something...
};


// Function prototypes
expr* newNumExpr(int val);
expr* newCharExpr(char val);
expr* newStrExpr(char* val);

#endif /* EXPRESSION_H_ */

