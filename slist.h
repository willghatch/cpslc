
/* slist.h
 * Simple linked list, because I want to just use C (for fun, and pedagogical reasons, just roll with it)
 * and I'm not sure I can use libraries like Glib or something that have nice data structures for me,
 * so I'm writing my own, to everybody's horror.
 */

typedef struct slist_struct slist;
struct slist_struct {
    slist* next;
    void* data;
};

