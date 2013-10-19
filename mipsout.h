// mipsout.h

#ifndef MIPSOUT_H
#define MIPSOUT_H

#include "slist.h"
#include "expression.h"


#define CONST_STR_LABEL "m_constStr"
#define BRANCH_LABEL "m_branch"
#define LOAD_CONST_STRLEN 50
#define CONST_STRDEF_STRLEN 50
#define OPERATOR_STRLEN 50
#define CHAR_PRINT_LABEL "m_charprint"

#define SYSC_PRINT_INT 1
#define SYSC_PRINT_STR 4
#define SYSC_READ_INT 5
#define SYSC_READ_STR 8
#define SYSC_EXIT 10

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
void m_bin_op_to_r1(char* opstr, int r1, int r2);
void m_bin_op_to_r3(char* opstr, int r1, int r2, int r3);
void m_write_str_index(int strIndex);
void m_write_str_reg(int reg);
void m_write_int(int reg);
void m_write_char(int reg);
void m_write_expr(expr* e);
void m_writeExpressionList(slist* ls);
void m_add_main_label();
void m_write_file(char* file);

extern int branchLabelIndex;
extern int strConstIndex;

#endif //MIPSOUT_H
