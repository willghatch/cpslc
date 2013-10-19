// mipsout.h

#ifndef MIPSOUT_H
#define MIPSOUT_H


void mips_init();
void m_add_data(char* str);
void m_add_text(char* str);
void m_add_main(char* str);
slist* m_get_data();
slist* m_get_text();
slist* m_get_main();


#endif //MIPSOUT_H
