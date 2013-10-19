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

void freeSlist(slist* list) {
    if (list == null) {
        return;
    }
    slist* n = list->next;
    free(list);
    freeSlist(next);
}

void hts_append(htslist* l, void* data) {
    slist* node = mkSlist(data);
    if (l->head == null) {
        l->head = node;
        l->tail = node;
        return;
    }
    l->tail->next = node;
    l->tail = node;
    return;
}

htslist* mkHtslist() {
    htslist* l;
    l = malloc(sizeof(htslist));
    l->head = null;
    l->tail = null;
    return l;
}

