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
    if (list == NULL) {
        return;
    }
    slist* n = list->next;
    free(list);
    freeSlist(n);
}

void hts_append(htslist* l, void* data) {
    slist* node = mkSlist(data);
    if (l->head == NULL) {
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
    l->head = NULL;
    l->tail = NULL;
    return l;
}

void slist_append(slist* ls, void* elem) {
    if (ls == NULL) {
        // error
    } else if (ls->next == NULL) {
        ls->next = mkSlist(elem);
    } else {
        slist_append(ls->next, elem);
    }
}

