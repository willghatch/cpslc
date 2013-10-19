// expressions.c

#include "symtab.h"
#include "expression.h"
#include <stdlib.h>
#include "register.h"
#include "mipsout.h"

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
    e->str_const_index = str_const_index;
    m_add_string_constant(val);
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

expr* newBinOpExpr(openum op, expr* e1, expr* e2) {
    expr* e;
    e = malloc(sizeof(expr));
    e->kind = operation_bin;
    e->edata.opdata.op = op;
    e->edata.opdata.operand1 = e1;
    e->edata.opdata.operand2 = e2;
    return e;
}

expr* newUnOpExpr(openum op, expr* e1) {
    expr* e;
    e = newBinOpExpr(op, e1, NULL);
    e->kind = operation_un;
    return e;
}

int evalExpr(expr* e) {
// returns a register number for the register that the output will be in
// TODO - do I want to free the expression here?  Not that I care about memory leaks much in this.
    int reg = -1;
    switch(e->kind) {
        case constant_expr:
            reg = getReg(registerState);
            m_load_constant(e, reg);
            break;
        case operation_bin:
            reg = doBinaryOperator(e->edata.opdata.op, e->edata.opdata.operand1, e->edata.opdata.operand2);
            break;
        case operation_un:
            reg = doUnaryOperator(e->edata.opdata.op, e->edata.opdata.operand1)
            break;
        default:
            break;
    }
    return reg;
}

int doBinaryOperator(openum op, expr* operand1, expr* operand2) {
    int reg1 = evalExpr(operand1);
    int reg2 = evalExpr(operand2);
    switch(op) {
    // Append a string to the mips output list to do the operation.
        case op_and:
            m_bin_op_to_r1("and", reg1, reg2);
            freeReg(registerState, reg2);
            break;
        case op_or: 
            m_bin_op_to_r1("or", reg1, reg2);
            freeReg(registerState, reg2);
            break;
        case op_add: 
            m_bin_op_to_r1("add", reg1, reg2);
            freeReg(registerState, reg2);
            break;
        case op_sub: 
            m_bin_op_to_r1("sub", reg1, reg2);
            freeReg(registerState, reg2);
            break;
        case op_mult: 
            m_mult_op(reg1, reg2, reg1);
            freeReg(registerState, reg2);
            break;
        case op_div: 
            m_divide_op(reg1, reg2, reg1);
            freeReg(registerState, reg2);
            break;
        case op_mod: 
            m_modulo_op(reg1, reg2, reg1);
            freeReg(registerState, reg2);
            break;
        case op_equal: 
            // TODO - implement these.
            // I need to use branch statements I think... 
            break;
        case op_nequal: 
            break;
        case op_gt: 
            break;
        case op_lt: 
            break;
        case op_gte: 
            break;
        case op_lte:
            break;
    }
    return reg1;
}

int doUnaryOperator(openum op, expr* operand) {
    int reg1 = evalExpr(operand1);
    switch(op) {
    // Append a string to the mips output list to do the operation.
        case op_not: 
            m_not(reg1, reg1);
            break;
        case op_negate: 
            m_negate(reg1, reg1);
            break;
    }
    return reg1;
}

