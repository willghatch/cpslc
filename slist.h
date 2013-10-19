
/* slist.h
 * Simple linked list, because I want to just use C (for fun, and pedagogical reasons, just roll with it)
 * and I'm not sure I can use libraries like Glib or something that have nice data structures for me,
 * so I'm writing my own, to everybody's horror.
 */

#ifndef SLIST_H
#define SLIST_H

typedef struct slist_struct slist;
struct slist_struct {
    slist* next;
    void* data;
};


slist* mkSlist(void* datum);
void freeSlist(slist* list);


typedef struct htslist_struct htslist;
struct htslist_struct {
    slist* head;
    slist* tail;
};


void hts_append(htslist* l, void* data);
htslist* mkHtslist();


#endif //SLIST_H

