// expressions.c

#include "symtab.h"
#include "expression.h"
#include <stdlib.h>
#include "register.h"
#include "mipsout.h"
#include "commonstuff.h"

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
    e->str_const_index = strConstIndex;
    m_add_string_constant(val);
    return e;
}

expr* newBoolExpr(int val) {
    expr* e;
    e = malloc(sizeof(expr));
    e->type = bool_type;
    e->kind = constant_expr;
    e->bool_val = val;
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

expr* newVariableExpr(ID* id, int isGlobal) {
    expr* e;
    e = malloc(sizeof(expr));
    e->type = id->id_type;
    e->kind = isGlobal ? globalVar : localVar;
    e->edata.id = id;
    return e;
}
expr* newGlobalVExpr(ID* id) {
    return newVariableExpr(id, 1);
}

expr* newLocalVExpr(ID* id) {
    return newVariableExpr(id, 0);
}

expr* newBinOpExpr(openum op, expr* e1, expr* e2) {
    expr* e;
    e = malloc(sizeof(expr));
    e->kind = operation_bin;
    e->edata.opdata.op = op;
    e->edata.opdata.operand1 = e1;
    e->edata.opdata.operand2 = e2;
    switch(op) {
        case op_and: 
        case op_or:
        case op_not:
        case op_equal:
        case op_nequal:
        case op_gt:
        case op_lt:
        case op_gte:
        case op_lte:
            e->type = bool_type;
            break;
        case op_negate:
        case op_add:
        case op_sub:
        case op_mult:
        case op_div:
        case op_mod:
            e->type = int_type;
            break;
        default:
            yyerror("unknown operator in expression creation... dying...");
            break;
    }
    return e;
}

expr* newUnOpExpr(openum op, expr* e1) {
    expr* e;
    e = newBinOpExpr(op, e1, NULL);
    e->kind = operation_un;
    return e;
}

expr* newFuncCallExpr(statement* procstmt) {
    expr* e = malloc(sizeof(expr));
    char* name = procstmt->data.procdata.name;
    ID* id = scopeLookup(name);
    if (id == NULL) {
        yyerror("unknown function name");
    }
    e->type = id->id_type;
    e->kind = functionCall;
    e->edata.funcCall = procstmt;
    return e;
}

int evalExpr(expr* e) {
// returns a register number for the register that the output will be in
// TODO - do I want to free the expression here?  Not that I care about memory leaks much in this.
// probably don't want to be too aggressive at freeing, because
// there are some global expressions (true, false)
    int reg = -1;
    TYPE* t = e->type;
    switch(e->kind) {
        case constant_expr:
            reg = getReg(registerState);
            m_load_constant(e, reg);
            break;
        case operation_bin:
            reg = doBinaryOperator(e->edata.opdata.op, e->edata.opdata.operand1, e->edata.opdata.operand2);
            break;
        case operation_un:
            reg = doUnaryOperator(e->edata.opdata.op, e->edata.opdata.operand1);
            break;
        case globalVar:
            reg = getReg(registerState);
            if(isWord_p(t)) {
                m_load_global_word(e->edata.id->id_label, reg);
            } else if (isByte_p(t)) {
                m_load_global_byte(e->edata.id->id_label, reg);
            }
            // TODO - handle more types
            break;
        case localVar:
            reg = getReg(registerState);
            int offset = e->edata.id->id_addr;
            if(isWord_p(t)) {
                m_load_frame_word(reg, offset, 0, 0);
            } else if (isByte_p(t)) {
                m_load_frame_word(reg, offset, 1, 0);
            }
            // TODO - handle user types
            break;
        case functionCall:
            if(isWord_p(t) || isByte_p(t)) {
                stmt_eval(e->edata.funcCall);
                reg = getReg(registerState);
                m_load_frame_word(reg, -t->ty_size, isByte_p(t), 1);
                m_move_stack_ptr(-t->ty_size);
            }
            // TODO - handle user types
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
            m_compare_mips_op("beq", reg1, reg2, reg1);
            freeReg(registerState, reg2);
            break;
        case op_nequal: 
            m_compare_mips_op("bne", reg1, reg2, reg1);
            freeReg(registerState, reg2);
            break;
        case op_gt: 
            m_compare_mips_op("bgtz", reg1, reg2, reg1);
            freeReg(registerState, reg2);
            break;
        case op_lt: 
            m_compare_mips_op("bltz", reg1, reg2, reg1);
            freeReg(registerState, reg2);
            break;
        case op_gte: 
            m_compare_mips_op("bgez", reg1, reg2, reg1);
            freeReg(registerState, reg2);
            break;
        case op_lte:
            m_compare_mips_op("blez", reg1, reg2, reg1);
            freeReg(registerState, reg2);
            break;
    }
    return reg1;
}

int doUnaryOperator(openum op, expr* operand) {
    int reg1 = evalExpr(operand);
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

