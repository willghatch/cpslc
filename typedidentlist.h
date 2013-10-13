// typedidentlist.h

#ifndef TYPEDIDENTLIST_H
#define TYPEDIDENTLIST_H

#include"slist.h"

typedef struct typedidentlist_struct typedidentlist;
struct typedidentlist_struct {
    TYPE* type;
    slist* names;
};

#endif //TYPEDIDENTLIST_H
