
#include "slist.h"

// These are all slists of strings
htslist* m_data;
htslist* m_text;
htslist* m_main;

void mips_init() {
    m_data = mkHtslist();
    m_text = mkHtslist();
    m_main = mkHtslist();
}


void m_add_data(char* str) {
    hts_append(m_data, str);
}

void m_add_text(char* str) {
    hts_append(m_text, str);
}

void m_add_main(char* str) {
    hts_append(m_main, str);
}

slist* m_get_data() {
    return m_data->head;
}

slist* m_get_text() {
    return m_text->head;
}

slist* m_get_main() {
    return m_main->head;
}

