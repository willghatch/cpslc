// function.c

#include "function.h"
#include "typedidentlist.h"
#include "commonstuff.h"
#include "mipsout.h"
#include "statement.h"
#include <stdlib.h>
#include <string.h>

// counter for numberical labels for functions
int fplabelcounter = 1;

int getFPLabel() {
    return fplabelcounter++;
}

int typedidentlists_equalp(typedidentlist* l, typedidentlist* r) {
// returns 1 if they're equal, else 0
    if (l->type != r->type) {
        return 0;
    }
    slist* nl = l->names;
    slist* nr = r->names;
    while(nl != NULL && nr != NULL) {
        if (strcmp(nl->data, nr->data)) {
            return 0;
        }
        nl = nl->next;
        nr = nr->next;
    }
    if (nl != NULL || nr != NULL) {
        return 0;
    }
    return 1;
}

int typedIdentList_list_equalp(slist* l,  slist* r) {
// takes two slist*'s of typedidentlists
// Returns true if the params are equivalent
    while(l != NULL && r != NULL) {
        if(!typedidentlists_equalp(l->data, r->data)) {
            return 0;
        }
        l = l->next;
        r = r->next;
    }
    if(l != NULL || r != NULL) {
        return 0;
    }
    return 1;
}

int calcSize_typedIdentList_list(slist* params) {
// Calculates the size of formal parameters
    int size = 0;
    while (params != NULL) {
        typedidentlist* til = params->data;
        TYPE* ty = til->type;
        int tysize = ty->ty_size;
        slist* names = til->names;
        while (names != NULL) {
            size += tysize;
            names = names->next;
        }
        params = params->next;
    }
    
    return size;
}

void declareFunc(char* name, slist* params, htslist* body, TYPE* t) {
    // Check if ID already exists (may have been forward declared)
    // Lookup the function in the scope below the current one.
    ID* func = IDsearch(name, scope[currscope-1]);
    int flabel, predeclared = 0;
    ID_KIND funcproc = t == NULL ? Procedure : Function;

    if (func != NULL) {
        predeclared = 1;
        if (func->id_kind != funcproc) {
            yyerror("Identifier already defined");
        } else if (!typedIdentList_list_equalp(func->typedIdentLists, params)) {
            yyerror("Formal parameters differ between function declarations");
        } else {
            flabel = func->id_label;
        }
    } else {
        flabel = getFPLabel();
        func = newid(name);
        func->id_kind = funcproc;
        func->id_type = t;
        func->typedIdentLists = params;
        func->id_label = flabel;
        func->param_size = calcSize_typedIdentList_list(params);
        addIdToTable_noAddrMove(func, scope+(currscope-1));
        printf("added id to table: %s\n", name);
    }
    // Write code for body
    if (body != NULL) {

        // So my calling convention will be:
        // 1 - reserve space on the stack for the return value
        // 2 - push all the registers (because who cares about performance anyway)
        // 3 - put parameters on the stack
        // 4 - when the callee exits it will put the return value in the reserved space and pop all the paramaters and locally stack variables
        // 5 - the caller will pop all the registers back off
        // 6 - the caller will use and clean up the return value

        // Make label for function
        m_add_function_label(flabel);
        // Caller will put params on stack, but I need to reserve space for the other vars
        int paramSize = calcSize_typedIdentList_list(params);
        int fullScopeSize = getSizeOfScopeVars(scope[currscope]);
        int nonParamSize = fullScopeSize - paramSize;
        m_move_stack_ptr(nonParamSize);
        // set frame pointer to the stack pointer, minus the frame's variable size
        m_set_fp_to_sp(-fullScopeSize);
        // write out the body of the function
        eval_stmt_list(body);
        // TODO - I should probably check somehow that I'll actually get a return statement... how shall I return stuff, exactly?

        m_function_end(flabel); // return statements will go to the end label, which will pop the stack frame
    }
}

int funcArgListMatches_p(ID* function, slist* paramExprs) {
// returns 0 if the parameters given don't match the function parameter types and length
    slist* idlists = function->typedIdentLists;
    while (idlists != NULL) {
        typedidentlist* idlist = idlists->data;
        TYPE* t = idlist->type;
        slist* names = idlist->names;
        while (names != NULL) {
            if (paramExprs == NULL) {
                return 0;
            }
            expr* e = paramExprs->data;
            if (e->type != t) {
                return 0;
            }
            paramExprs = paramExprs->next;
            names = names->next;
        }
        idlists = idlists->next;
    }
    return paramExprs == NULL;
}


