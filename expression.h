
#ifndef EXPRESSION_H_
#define EXPRESSION_H_

// So I need something to hold values for expressions...

#include "symtab.h"

// Forward declare statement from statement.h, of which this file is a dependency
typedef struct stmt_struct statement;

typedef enum operator_enum openum;
enum operator_enum {
    op_and, op_or, op_not, 
    op_negate, op_add, op_sub, op_mult, op_div, op_mod, 
    op_equal, op_nequal, op_gt, op_lt, op_gte, op_lte
};

typedef enum expr_kind_enum expr_kind;
enum expr_kind_enum {
    constant_expr, operation_bin, operation_un, registerVal, globalVar, localVar, functionCall, typecast
};

typedef struct expression_struct expr;
struct expression_struct {
    TYPE* type;
    expr_kind kind;
    int int_val;
    char char_val;
    int bool_val;
    char* str_val;
    // I should have some way to hold the value of generic types... but when
    // they're not constant I'll have to have part of the parse tree or something...
    int str_const_index; // index in the list of string constants
    expr* offsetExpr;
    int pointer_p;
    union {
        // TODO - move these values into the union.
        //int int_val;
        //char char_val;
        //bool bool_val;
        //char* str_val;
        struct {
            expr* operand1;
            expr* operand2;
            enum operator_enum op;
        } opdata;
        int reg_number;
        ID* id;
        statement* funcCall;
        expr* innerExpr;
    } edata;
    
};


// Function prototypes
expr* newNumExpr(int val);
expr* newCharExpr(char val);
expr* newStrExpr(char* val);
expr* newBoolExpr(int val);
expr* newRegExpr(int reg, TYPE* type);
expr* newBinOpExpr(openum op, expr* e1, expr* e2);
expr* newUnOpExpr(openum op, expr* e1);
expr* newGlobalVExpr(ID* id);
expr* newLocalVExpr(ID* id);
expr* newFuncCallExpr(statement* procstmt);
expr* newCastExpr(TYPE* newtype, expr* toCast);
int evalExpr(expr* e);
int evalExprToPointer(expr* e);
int doBinaryOperator(enum operator_enum op, expr* operand1, expr* operand2);
int doUnaryOperator(enum operator_enum op, expr* operand1);

#endif /* EXPRESSION_H_ */

