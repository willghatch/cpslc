// typedidentlist.h

#ifndef TYPEDIDENTLIST_H
#define TYPEDIDENTLIST_H

#include "slist.h"
#include "symtab.h"

typedef struct typedidentlist_struct typedidentlist;
struct typedidentlist_struct {
    TYPE* type;
    int pointerP;
    slist* names;
};

#endif //TYPEDIDENTLIST_H
