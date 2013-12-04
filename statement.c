// statement.c

#include <stdlib.h>
#include "statement.h"
#include "function.h"
#include "mipsout.h"
#include "commonstuff.h"

conditional* mkConditional(expr* cond, BranchType btype, htslist* stmts) {
    conditional* c = malloc(sizeof(conditional));
    c->condition = cond;
    c->branchType = btype;
    c->statements = stmts;
    return c;
    // TODO - should I care about leaking memory with these things?  I haven't cared much so far.
    // Frankly, all these tree nodes are just going to pile up until I have the full syntax tree,
    // then all be written out and the process will exit... so it shouldn't matter.
}

statement* mkIfStmt(htslist* conditionals) {
    statement* s = malloc(sizeof(statement));
    s->type = if_stmt;
    s->data.if_conditionals = conditionals;
    return s;
}

statement* mkAssignmentStmt(expr* lvalue, expr* rvalue) {
    statement* s = malloc(sizeof(statement));
    s->type = assign_stmt;
    s->data.assigndata.lvalue = lvalue;
    s->data.assigndata.rvalue = rvalue;
    return s;
}

statement* mkIncrementStmt(expr* lvalue, inctype type) {
    openum op;
    if(type == Increment) {
        op = op_add;
    } else if (type == Decrement) {
        op = op_sub;
    } else {
        return NULL;
    }
    expr* rvalue = newBinOpExpr(op, lvalue, one_expr);
    return mkAssignmentStmt(lvalue, rvalue);
}

statement* mkReadStmt(slist* exprList) {
    statement* s = malloc(sizeof(statement));
    s->type = read_stmt;
    s->data.exprList = exprList;
    return s;
}

statement* mkWriteStmt(slist* exprList) {
    statement* s = malloc(sizeof(statement));
    s->type = write_stmt;
    s->data.exprList = exprList;
    return s;
}

statement* mkLoopStmt(statement* init, conditional* cond, stmt_type looptype) {
    statement* s = malloc(sizeof(statement));
    s->type = looptype;
    s->data.loopdata.init = init;
    s->data.loopdata.cBlock = cond;
    return s;
}

statement* mkForStmt(statement* assign, inctype updown, expr* finalVal, htslist* stmts) {
    expr* lval = assign->data.assigndata.lvalue;
    statement* update = mkIncrementStmt(lval, updown);
    openum op = updown == Increment ? op_gt : op_lt;
    expr* condition = newBinOpExpr(op, lval, finalVal);
    conditional* cond = mkConditional(condition, bt_equal0, stmts);
    hts_append(cond->statements, update);
    statement* s = mkLoopStmt(assign, cond, for_stmt);
    return s;
}

statement* mkRepeatStmt(expr* condition, htslist* stmts) {
    conditional* c = mkConditional(condition, bt_equal0, stmts);
    return mkLoopStmt(NULL, c, repeat_stmt);
}

statement* mkWhileStmt(expr* condition, htslist* stmts) {
    conditional* c = mkConditional(condition, bt_nequal0, stmts);
    return mkLoopStmt(NULL, c, while_stmt);
}

statement* mkStopStmt() {
    statement* s = malloc(sizeof(statement));
    s->type = stop_stmt;
    return s;
}

statement* mkProcedureStmt(char* name, slist* paramExprs) {
    statement* s = malloc(sizeof(statement));
    s->type = proc_stmt;
    s->data.procdata.name = name;
    s->data.procdata.paramExprs = paramExprs;
    if(scopeLookup(name) == NULL) {
        yyerror("Unknown function/procedure name");
    }
    return s;
}

statement* mkReturnStmt(expr* e) {
    statement* s = malloc(sizeof(statement));
    s->type = return_stmt;
    s->data.expression = e;
    return s;
}


//// Statement Evaluation:
void stmt_eval_assign(statement* s) {
    m_assign_stmt(s->data.assigndata.lvalue, s->data.assigndata.rvalue);
}

void stmt_eval_if(statement* s) {
    m_if_stmt(s->data.if_conditionals);
}

void stmt_eval_while(statement* s) {
    m_while_stmt(s->data.loopdata.cBlock);
}

void stmt_eval_repeat(statement* s) {
    m_repeat_stmt(s->data.loopdata.cBlock);
}

void stmt_eval_for(statement* s) {
    m_for_stmt(s->data.loopdata.init, s->data.loopdata.cBlock);
}

void stmt_eval_stop(statement* s) {
    m_stop_stmt();
}

void stmt_eval_return(statement* s) {
    expr* e = s->data.expression;
    if (e != NULL) {
        // push return value to stack
        m_store_ret_val(e);
    }
    m_function_end();
}

void stmt_eval_read(statement* s) {
    slist* lvalExprs = s->data.exprList;
    m_read_stmt(lvalExprs);
}

void stmt_eval_write(statement* s) {
    slist* lvalExprs = s->data.exprList;
    m_write_stmt(lvalExprs);
}

void stmt_eval_proc(statement* s) {
    char* name = s->data.procdata.name;
    slist* paramExprs = s->data.procdata.paramExprs;
    ID* proc = scopeLookup(name);
    if (proc == NULL) {
        yyerror("Unknown function or procedure name");
    }
    TYPE* t = proc->id_type;
    funcArgList_checkAndAddPointerPs(proc, paramExprs);
    int funcLabel = proc->id_label;
    m_func_call(funcLabel, paramExprs, t);
}

void stmt_eval(statement* s) {
    if (s == NULL) {
        return;
    }
    stmt_type t = s->type;
    switch(t) {
        case assign_stmt:
            stmt_eval_assign(s);
            return;
        case if_stmt:
            stmt_eval_if(s);
            return;
        case while_stmt:
            stmt_eval_while(s);
            return;
        case repeat_stmt:
            stmt_eval_repeat(s);
            return;
        case for_stmt:
            stmt_eval_for(s);
            return;
        case stop_stmt:
            stmt_eval_stop(s);
            return;
        case return_stmt:
            stmt_eval_return(s);
            return;
        case read_stmt:
            stmt_eval_read(s);
            return;
        case write_stmt:
            stmt_eval_write(s);
            return;
        case proc_stmt:
            stmt_eval_proc(s);
            return;
        default:
            return;
    }
}

void eval_stmt_list(htslist* stmts) {
    if (stmts == NULL) {
        return;
    }
    slist* l = stmts->head;
    while(l != NULL) {
        stmt_eval(l->data);
        l = l->next;
    }
    // TODO - I can probably free the list as well as the nodes here... If I decide I care about that.
}

