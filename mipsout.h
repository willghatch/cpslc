// mipsout.h

#ifndef MIPSOUT_H
#define MIPSOUT_H

#include "slist.h"
#include "expression.h"
#include "statement.h"


#define CONST_STR_LABEL "m_constStr"
#define BRANCH_LABEL "m_branch"
#define LOAD_CONST_STRLEN 50
#define CONST_STRDEF_STRLEN 50
#define OPERATOR_STRLEN 50
#define CHAR_PRINT_LABEL "m_charprint"
#define GLOBAL_VAR_LABEL "m_globalvar"
#define STR_READ_LABEL "m_strRead"
#define STR_READ_LEN 500
#define FALSE_STR_LABEL "m_false_str"
#define TRUE_STR_LABEL "m_true_str"
#define STACK_SPACE_LABEL "m_stack_space"
#define STACK_SIZE 10000
#define FUNC_LABEL "m_function"
#define SP_REG_NUM 29
#define FP_REG_NUM 30
#define RA_REG_NUM 31
#define PUSH_REG_MIN 2
#define PUSH_REG_MAX 31
#define WORDSIZE 4

#define SYSC_PRINT_INT 1
#define SYSC_PRINT_STR 4
#define SYSC_READ_INT 5
#define SYSC_READ_STR 8
#define SYSC_EXIT 10
#define SYSC_READ_CHAR 12

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
void m_write_bool(int reg);
void m_write_expr(expr* e);
void m_add_main_label();
void m_write_file(char* file);
int m_reserve_global_var(int size);
void m_load_global_byte(int index, int reg);
void m_load_global_word(int index, int reg);
void m_load_global_address(int index, int reg);
void m_read_int(int reg);
void m_read_str(int reg, int size);
void m_assign_int_global(int reg, int globalIndex);
void m_read_expr_int(ID* intvar);
void m_compare_mips_op(char* opstr, int r_l, int r_r, int r_dest);
// statement functions
void m_assign_stmt(expr* lval, expr* rval);
void m_if_stmt(htslist* conditionals);
void m_for_stmt(statement* init, conditional* c);
void m_while_stmt(conditional* c);
void m_repeat_stmt(conditional* c);
void m_stop_stmt();
void m_read_stmt(slist* ls);
void m_write_stmt(slist* ls);
void m_add_function_label(int FLabelNum);
void m_move_stack_ptr(int size);
void m_set_fp_to_sp(int offsetFromSp);
void m_push_all_regs();
void m_pop_all_regs();
void m_function_end();
void m_func_call(int funcLabel, slist* paramExprs, TYPE* t);
void m_push_parameter_exprs(slist* paramExprs);
void m_load_frame_word(int reg, int offset, int justByte, int useSPinsteadOfFP, int overrideReg);
void m_store_ret_val(expr* e);
void m_copy_fp(int destReg);
void m_load_word_from_addr(int destReg, int addrReg, int staticOffset, int byteOnlyP);
void m_copy_reg(int dstReg, int srcReg);
void m_copyMem(int srcAddrReg, int dstAddrReg, int size);

extern int branchLabelIndex;
extern int strConstIndex;

#endif //MIPSOUT_H
