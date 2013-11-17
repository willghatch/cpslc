// statement.h

#ifndef STATEMENT_H
#define STATEMENT_H

#include "expression.h"
#include "slist.h"

// Increment type enum for loops
typedef enum inctype_enum inctype;
enum inctype_enum {
    Increment, Decrement, Nocrement // Nocrement is for no-op
};

typedef enum stmt_type_enum stmt_type;
enum stmt_type_enum {
    assign_stmt, if_stmt, while_stmt, repeat_stmt, for_stmt, 
    stop_stmt, return_stmt, read_stmt, write_stmt, proc_stmt, 
    null_stmt
};

typedef enum BranchType_enum BranchType;
enum BranchType_enum {
    bt_equal0, bt_nequal0
};

typedef struct conditional_struct conditional;
struct conditional_struct {
    // a null condition will mean no condition.
    expr* condition;
    BranchType branchType;
    htslist* statements;
};

typedef struct stmt_struct statement;
struct stmt_struct {
    stmt_type type;
    union {
        htslist* if_conditionals;
        slist* exprList; // for read/write stmts
        struct {
            statement* init;
            conditional* cBlock;
        } loopdata;
        struct {
            expr* lvalue;
            expr* rvalue;
        } assigndata;
        struct {
            char* name;
            slist* paramExprs;
        } procdata;
    } data;
};



// Functions
conditional* mkConditional(expr* cond, BranchType btype, htslist* stmts);
statement* mkIfStmt(htslist* conditionals);
statement* mkAssignmentStmt(expr* lvalue, expr* rvalue);
statement* mkIncrementStmt(expr* lvalue, inctype type);
statement* mkReadStmt(slist* exprList);
statement* mkWriteStmt(slist* exprList);
statement* mkLoopStmt(statement* init, conditional* cond, stmt_type looptype);
statement* mkForStmt(statement* assign, inctype updown, expr* finalVal, htslist* stmts);
statement* mkRepeatStmt(expr* condition, htslist* stmts);
statement* mkWhileStmt(expr* condition, htslist* stmts);
statement* mkStopStmt();
statement* mkProcedureStmt(char* name, slist* paramExprs);
void stmt_eval(statement* s);
void eval_stmt_list(htslist* stmts);



#endif //STATEMENT_H

