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
void m_add_string_constant(char* str);
void m_load_constant(expr* e, int reg);
void m_bin_op_to_r1(char* opstr, int r1, int r2);
void m_bin_op_to_r3(char* opstr, int r1, int r2, int r3);
void m_divide_op(int rTop, int rBot, int rDest);
void m_modulo_op(int rTop, int rBot, int rDest);
void m_mult_op(int r1, int r2, int rDest);
void m_not(int dest, int src);
void m_negate(int dest, int src);

extern int branchLabelIndex;
extern int strConstIndex;

#endif //MIPSOUT_H