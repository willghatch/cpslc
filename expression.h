
#ifndef EXPRESSION_H_
#define EXPRESSION_H_

// So I need something to hold values for expressions...

#include"symtab.h"

enum operator_enum {
    op_and, op_or, op_not, 
    op_negate, op_add, op_sub, op_mult, op_div, op_mod, 
    op_equal, op_nequal, op_gt, op_lt, op_gte, op_lte
}

typedef enum expr_kind_enum expr_kind;
enum expr_kind_enum {
    constant_expr, operation_bin, operation_un, registerVal, globalVar, localVar, parameter
};

typedef struct expression_struct expr;
struct expression_struct {
    TYPE* type;
    expr_kind kind;
    int int_val;
    char char_val;
    bool bool_val;
    char* str_val;
    // I should have some way to hold the value of generic types... but when
    // they're not constant I'll have to have part of the parse tree or something...
    int str_const_index; // index in the list of string constants
    int reg_number;
    union {
        int int_val;
        char char_val;
        bool bool_val;
        struct {
            expr* operand1;
            expr* operand2;
            enum operator_enum op;
        } opdata;
        struct {
        } funcdata;
    } edata;
    
};


// Function prototypes
expr* newNumExpr(int val);
expr* newCharExpr(char val);
expr* newStrExpr(char* val);
expr* newBoolExpr(bool val);
expr* newRegExpr(int reg, TYPE* type);
int doOperator(enum operator_enum op, expr* operand1, expr* operand2);

#endif /* EXPRESSION_H_ */

