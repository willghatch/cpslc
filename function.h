// function.h

#ifndef FUNCTION_H
#define FUNCTION_H

#include "slist.h"
#include "symtab.h"

int getFPLabel();
int typedIdentList_list_equalp(slist* l,  slist* r);
int calcSize_typedIdentList_list(slist* params);
void declareFunc(char* name, slist* params, htslist* body, TYPE* t);
int funcArgListMatches_p(ID* function, slist* paramExprs);

#endif //FUNCTION_H
