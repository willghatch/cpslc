// typedidentlist.h

#ifndef TYPEDIDENTLIST_H
#define TYPEDIDENTLIST_H

#include"slist.h"

typedef struct typedidentlist_struct typedidentlist;
struct typedidentlist_struct {
    ID* type_id;
    slist* names;
};

#endif //TYPEDIDENTLIST_H
