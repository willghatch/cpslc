// slist.c

#include "slist.h"
#include <stdlib.h>

slist* mkSlist(void* datum) {
    slist* ls;
    ls = malloc(sizeof(slist));
    ls->data = datum;
    ls->next = NULL;
    return ls;
}

