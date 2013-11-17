
#include "mipsout.h"
#include "slist.h"
#include "expression.h"
#include "register.h"
#include "commonstuff.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



// These are all slists of strings
htslist* m_data;
htslist* m_text;

int strConstIndex = 0;
int branchLabelIndex = 0;
int globalVarIndex = 0;

void mips_init() {
    m_data = mkHtslist();
    m_text = mkHtslist();
}


void m_add_data(char* str) {
    hts_append(m_data, str);
}

void m_add_text(char* str) {
    hts_append(m_text, str);
}

slist* m_get_data() {
    return m_data->head;
}

slist* m_get_text() {
    return m_text->head;
}

int upper16(int n) {
// returns upper 16 bits of a 32 bit int
    return (n >> 16) & 0xFFFF;
}

int lower16(int n) {
// returns lower 16 bits of a 32 bit int
    return n & 0xFFFF;
}

char* strdup_mips_escaped(char* str) {
// strdup, only it escapes \n and \t characters that spim handles
    char* newstr;
    int len = strlen(str);
    int numEsc = 0;
    for(int i = 0; i < len; ++i) {
        switch(str[i]) {
            case '\n':
            case '\t':
            case '\"':
            // It seems spim only handles these two escape sequences.
                ++numEsc;
                break;
            default:
                break;
        }
    }
    newstr = malloc((len + numEsc) * sizeof(char));

    int nsi = 0; // new string i
    for(int i = 0; i < len; ++i) {
        switch(str[i]) {
            case '\n':
                newstr[nsi++] = '\\';
                newstr[nsi++] = 'n';
                break;
            case '\t':
                newstr[nsi++] = '\\';
                newstr[nsi++] = 't';
                break;
            case '\"':
                newstr[nsi++] = '\\';
                newstr[nsi++] = '\"';
                break;
            default:
                newstr[nsi++] = str[i];
                break;

        }
    }
    return newstr;
}

void m_align() { 
    // force alignment so spim doesn't yell
    m_add_data("\t.align 2\n");
}

void m_add_string_constant(char* str) {
    m_align();
    char* o = malloc(CONST_STRDEF_STRLEN*sizeof(char));
    snprintf(o, CONST_STRDEF_STRLEN, "%s%i:\n\t.asciiz \"", CONST_STR_LABEL, strConstIndex);
    m_add_data(o);
    m_add_data(strdup_mips_escaped(str));
    m_add_data("\"\n");

    // increment 
    ++strConstIndex;
}

void m_load_constant(expr* e, int reg) {
    char* o;

    if(e->type == int_type) {
        // Load immediate value in two steps - upper and lower 16 bits
        int n = e->int_val;
        o = malloc(LOAD_CONST_STRLEN*sizeof(char));
        snprintf(o, LOAD_CONST_STRLEN, "lui $%i, %i #loadint upper\n", reg, upper16(n));
        m_add_text(o);
        o = malloc(LOAD_CONST_STRLEN*sizeof(char));
        snprintf(o, LOAD_CONST_STRLEN, "ori $%i, $%i, %i #loadint lower\n", reg, reg, lower16(n));
        m_add_text(o);
    } else if (e->type == char_type) {
        char c = e->char_val;
        o = malloc(LOAD_CONST_STRLEN*sizeof(char));
        snprintf(o, LOAD_CONST_STRLEN, "li $%i, %i #loadchar\n", reg, c);
        m_add_text(o);
    } else if (e->type == bool_type) {
        int b = e->bool_val ? 1 : 0;
        o = malloc(LOAD_CONST_STRLEN*sizeof(char));
        snprintf(o, LOAD_CONST_STRLEN, "li $%i, %i #loadbool\n", reg, b);
        m_add_text(o);
    } else if (e->type == str_type) {
        // Load the address of the constant string by its index
        int snum = e->str_const_index;
        o = malloc(LOAD_CONST_STRLEN*sizeof(char));
        snprintf(o, LOAD_CONST_STRLEN, "la $%i, %s%i #loadstr\n", reg, CONST_STR_LABEL, snum);
        m_add_text(o);
    }
}

void m_bin_op_to_r1(char* opstr, int r1, int r2) {
    m_bin_op_to_r3(opstr, r1, r2, r1);
}

void m_bin_op_to_r3(char* opstr, int r1, int r2, int r3) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s $%i, $%i, $%i\n", opstr, r3, r1, r2);
    m_add_text(o);
}

void m_divide_op(int rTop, int rBot, int rDest) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "div $%i, $%i\n", rTop, rBot);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "mflo $%i\n", rDest);
    m_add_text(o);
}

void m_modulo_op(int rTop, int rBot, int rDest) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "div $%i, $%i\n", rTop, rBot);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "mfhi $%i\n", rDest);
    m_add_text(o);
}

void m_mult_op(int r1, int r2, int rDest) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "mult $%i, $%i\n", r1, r2);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "mflo $%i\n", rDest);
    m_add_text(o);
}

void m_not(int dest, int src) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "xori $%i, $%i, 1\n", dest, src);
    m_add_text(o);
}

void m_negate(int dest, int src) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "sub $%i, $0, $%i\n", dest, src);
    m_add_text(o);
}

// write statement
// need to write strings, ints, chars, and bools
// use syscall...

void m_write_expr(expr* e) {
    int reg;
    reg = evalExpr(e);
    if (e->type == int_type) {
        m_write_int(reg);
    } else if (e->type == str_type) {
        m_write_str_reg(reg);
    } else if (e->type == char_type) {
        m_write_char(reg);
    } else if (e->type == bool_type) {
        m_write_bool(reg);
    }
    freeReg(registerState, reg);
}

void m_write_str_index(int strIndex) {
    m_add_text("#writing string\n");
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "la $a0, %s%i\n", CONST_STR_LABEL, strIndex);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "li $v0, $%i\n", SYSC_PRINT_STR);
    m_add_text(o);
    m_add_text("syscall\n");
}

void m_write_str_reg(int reg) {
    m_add_text("#writing string\n");
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "move $a0, $%i\n", reg);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "li $v0, %i\n", SYSC_PRINT_STR);
    m_add_text(o);
    m_add_text("syscall\n");
}

void m_write_int(int reg) {
    m_add_text("#writing int\n");
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "move $a0, $%i\n", reg);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "li $v0, %i\n", SYSC_PRINT_INT);
    m_add_text(o);
    m_add_text("syscall\n");
}

void m_write_char(int reg) {
    m_add_text("#writing char\n");
    // have a special pseudo-constant string label that's one character long (plus null terminator),
    // load, modify, and print it every time for a character
    char* o;
    // get char label address
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "la $a0, %s\n", CHAR_PRINT_LABEL);
    m_add_text(o);
    // store 0 in space after char
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "sb $0, 1($a0)\n");
    m_add_text(o);
    // store character into char print label
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "sb $%i, 0($a0)\n", reg);
    m_add_text(o);
    // load print syscall
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "li $v0, %i\n", SYSC_PRINT_STR);
    m_add_text(o);
    m_add_text("syscall\n");
}

void m_write_bool(int reg) {
    // write "true" or "false" on 1 or 0...
    // bne <reg> $0 LABELXA
    // b LABELXB
    // LABELXA:
    // la $a0 TRUESTR
    // b LABELXC
    // LABELXB:
    // la $a0 FALSESTR
    // LABELXC:
    // li $v0 SYSC_PRINT_STR
    // syscall
    int bi = branchLabelIndex++;
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "bne $%i, $0 %s%iA\n", reg, BRANCH_LABEL, bi);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "b %s%iB\n", BRANCH_LABEL, bi);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s%iA:\n", BRANCH_LABEL, bi);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "la $a0 %s\n", TRUE_STR_LABEL);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "b %s%iC\n", BRANCH_LABEL, bi);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s%iB:\n", BRANCH_LABEL, bi);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "la $a0 %s\n", FALSE_STR_LABEL);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s%iC:\n", BRANCH_LABEL, bi);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "li $v0 %i\n", SYSC_PRINT_STR);
    m_add_text(o);
    m_add_text("syscall\n");
}


void m_add_main_label() {
    m_add_text("\nmain:\n");
}

void m_add_function_label(int FLabelNum) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "\n%s_%i:\n", FUNC_LABEL, FLabelNum);
    m_add_text(o);
}

void m_write_file(char* file) {
    FILE* f = fopen(file, "w");
    fprintf(f, "\n#File written by the (not so) amazing cpslc written by William Hatch\n");

    // write data section
    fprintf(f, "\n.data\n");
    // add special strings for true, false
    fprintf(f, "%s:\n\t.asciiz \"false\"\n", FALSE_STR_LABEL);
    fprintf(f, "%s:\n\t.asciiz \"true\"\n", TRUE_STR_LABEL);
    // add special character printing string
    m_align();
    fprintf(f, "%s:\n\t.space 2\n", CHAR_PRINT_LABEL);
    // Put stack space
    m_align();
    fprintf(f, "%s:\n\t.space %i\n", STACK_SPACE_LABEL, STACK_SIZE);
    slist* ls = m_data->head;
    while(ls != NULL) {
        fprintf(f, ls->data);
        ls = ls->next;
    }

    // write text section
    fprintf(f, "\n.text\n");
    // Initialize stack pointer
    fprintf(f, "#Init stack\n");
    fprintf(f, "la $sp %s\n\n", STACK_SPACE_LABEL);
    
    // Print the text list
    ls = m_text->head;
    while(ls != NULL) {
        fprintf(f, ls->data);
        ls = ls->next;
    }

    fprintf(f, "li $v0 10\nsyscall\n");

    // add gratuitous newlines.  I like it that way.
    fprintf(f, "\n\n");

    fclose(f);
}

int m_reserve_global_var(int size) {
// print out a label for a global variable.  Return the label number.
    int reservation = globalVarIndex++;
    char* o;
    m_align();
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s%i:\n\t.space %i\n", GLOBAL_VAR_LABEL, reservation, size);
    m_add_data(o);
    return reservation;
}

void m_load_global_address(int index, int reg) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "la $%i, %s%i #load global\n", reg, GLOBAL_VAR_LABEL, index);
    m_add_text(o);
}

void m_load_global_word(int index, int reg) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "lw $%i, %s%i #load global\n", reg, GLOBAL_VAR_LABEL, index);
    m_add_text(o);
}
void m_load_global_byte(int index, int reg) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "lb $%i, %s%i #load global\n", reg, GLOBAL_VAR_LABEL, index);
    m_add_text(o);
}

void m_read_int(int reg) {
    m_add_text("#reading int\n");
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "li $v0, %i\n", SYSC_READ_INT);
    m_add_text(o);
    m_add_text("syscall\n");
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "move $%i, $v0\n", reg);
    m_add_text(o);
}

void m_read_str(int reg, int size) {
    m_add_text("#reading string\n");
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "li $v0, %i\n", SYSC_READ_STR);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "move $a0, $%i\n", reg);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "li $a1, %i\n", size);
    m_add_text(o);
    m_add_text("syscall\n");
}

void m_store_word(int fromReg, int addrReg, int offset, int storeByteOnly) {
    char* o;
    char* op = storeByteOnly ? "sb" : "sw";
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s $%i, %i($%i)\n", op, fromReg, offset, addrReg);
    m_add_text(o);
}

void m_store_word_global(int reg, int globalIndex, int offset, int storeByteOnly) {
    m_add_text("#storing global\n");
    int tempreg = getReg(registerState);
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "la $%i, %s%i\n", tempreg, GLOBAL_VAR_LABEL, globalIndex);
    m_add_text(o);
    m_store_word(reg, tempreg, offset, 0);
    freeReg(registerState, tempreg);
}

void m_store_word_local(int reg, int offset, int storeByteOnly) {
    m_add_text("#storing local\n");
    m_store_word(reg, FP_REG_NUM, offset, storeByteOnly);
}

void m_read_expr_int(ID* intvar) {
    //read syscall
    //store
    int reg;
    reg = getReg(registerState);
    m_read_int(reg);
    if (isGlobal(intvar)) {
        m_store_word_global(reg, intvar->id_label, 0, 0);
    } else {
        m_store_word_local(reg, intvar->id_addr, 0);
    }

    freeReg(registerState, reg);
}

void m_read_expr(expr* e) {
    if (!(e->kind == globalVar || e->kind == localVar)) {
        yyerror("Read expression with a non-variable");
    }
    if(e->type == int_type) {
        m_read_expr_int(e->edata.id);
    } else if (e->type == char_type) {
        // TODO - support character reading
    } else {
        yyerror("Read expression with a type other than integer or character");
    }
}


void m_compare_mips_op(char* opstr, int r_l, int r_r, int r_dest) {
    m_add_text("#(in)equality comparison op\n");
    int bi = branchLabelIndex++;
    m_bin_op_to_r3("sub", r_l, r_r, r_dest);
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    // <branchop> # conditional branch based on desired operator to LABELXA
    if(!strcmp(opstr, "beq") || !strcmp(opstr, "bne")) {
    // beq and bne require a second register arg
        snprintf(o, OPERATOR_STRLEN, "%s $%i $0 %s%iA\n", opstr, r_dest, BRANCH_LABEL, bi);
    } else {
        snprintf(o, OPERATOR_STRLEN, "%s $%i %s%iA\n", opstr, r_dest, BRANCH_LABEL, bi);
    }
    m_add_text(o);
    // b LABELXB
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "b %s%iB\n", BRANCH_LABEL, bi);
    m_add_text(o);
    // LABELXA:
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s%iA:\n", BRANCH_LABEL, bi);
    m_add_text(o);
    // li <dest> 1 # true case
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "li $%i 1\n", r_dest);
    m_add_text(o);
    // b LABELXC
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "b %s%iC\n", BRANCH_LABEL, bi);
    m_add_text(o);
    // LABELXB:
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s%iB:\n", BRANCH_LABEL, bi);
    m_add_text(o);
    // li <dest> 0 # false case
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "li $%i 0\n", r_dest);
    m_add_text(o);
    // LABELXC:
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s%iC:\n", BRANCH_LABEL, bi);
    m_add_text(o);
    // And now we continue with the operator return value in <dest>
}


void m_write_labelled_stmts(int branchIndex, int subIndex, htslist* stmts, char* endGoto) {
// writes statement blocks with a label (for loops, ifs)
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s%i_%i:\n", BRANCH_LABEL, branchIndex, subIndex);
    m_add_text(o);
    eval_stmt_list(stmts);
    if (endGoto != NULL) {
        m_add_text(endGoto);
    }
}

void m_write_if_branchblock(int branchIndex, slist* conditionals) {
// Writes evaluation of conditions and branch statements for if statements
    slist* l = conditionals;
    char* o;
    int subIndex = 0;
    while (l != NULL) {
        conditional* c = l->data;
        if(c->condition == NULL) {
            o = malloc(OPERATOR_STRLEN*sizeof(char));
            snprintf(o, OPERATOR_STRLEN, "b %s%i_%i\n", BRANCH_LABEL, branchIndex, subIndex);
            m_add_text(o);
        } else {
            int reg = evalExpr(c->condition);
            char* bstr = c->branchType == bt_equal0 ? "beq" : "bne";
            o = malloc(OPERATOR_STRLEN*sizeof(char));
            snprintf(o, OPERATOR_STRLEN, "%s $%i $0 %s%i_%i\n", bstr, reg, BRANCH_LABEL, branchIndex, subIndex);
            m_add_text(o);
            freeReg(registerState, reg);
        }

        l = l->next;
        ++subIndex;
    }
}


//// Statement printing!

void m_assign_stmt(expr* lval, expr* rval) {
    if(lval->kind == globalVar && lval->type == int_type) {
        int reg = evalExpr(rval);
        int globalIndex = lval->edata.id->id_label;
        m_store_word_global(reg, globalIndex, 0, 0);
    }
    // TODO - deal with other lvalue types and kinds
}

void m_if_stmt(htslist* conditionals) {
    m_add_text("#If Statement\n");
    int bi = branchLabelIndex++;
    // Write header (expression evaluations)
    m_write_if_branchblock(bi, conditionals->head);
    // Write blocks
    slist* l = conditionals->head;
    int ifElseIndex = 0;
    char* endGoto = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(endGoto, OPERATOR_STRLEN, "b %s%i_end\n", BRANCH_LABEL, bi);
    while(l != NULL) {
        conditional* c = l->data;
        m_write_labelled_stmts(bi, ifElseIndex, c->statements, endGoto);
        ++ifElseIndex;
        l = l->next;
    }
    char* endLabel = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(endLabel, OPERATOR_STRLEN, "%s%i_end:\n", BRANCH_LABEL, bi);
    m_add_text(endLabel);
}

void m_for_stmt(statement* init, conditional* c) {
    m_add_text("#For Statement\n");
    int bi = branchLabelIndex++;
    // Put initializing statement
    stmt_eval(init);
    // Put condition label
    char* o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s%i_cond:\n", BRANCH_LABEL, bi);
    m_add_text(o);
    // Put conditional branch
    int reg = evalExpr(c->condition);
    char* bstr = c->branchType == bt_equal0 ? "beq" : "bne";
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s $%i $0 %s%i_0\n", bstr, reg, BRANCH_LABEL, bi);
    m_add_text(o);
    freeReg(registerState, reg);
    // If the condition failed, branch to end
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "b %s%i_end\n", BRANCH_LABEL, bi);
    m_add_text(o);
    // Write block
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "b %s%i_cond\n", BRANCH_LABEL, bi);
    m_write_labelled_stmts(bi, 0, c->statements, o);
    // write end label
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s%i_end:\n", BRANCH_LABEL, bi);
    m_add_text(o);
}

void m_while_stmt(conditional* c) {
    m_add_text("#While Statement\n");
    int bi = branchLabelIndex++;
    // Just do a for statement...
    m_for_stmt(NULL, c);
}

void m_repeat_stmt(conditional* c) {
    m_add_text("#Repeat Statement\n");
    int bi = branchLabelIndex++;

    // Put statement block (labelled)
    m_write_labelled_stmts(bi, 0, c->statements, NULL);
    // Put conditional jump back
    int reg = evalExpr(c->condition);
    char* bstr = c->branchType == bt_equal0 ? "beq" : "bne";
    char* o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s $%i $0 %s%i_0\n", bstr, reg, BRANCH_LABEL, bi);
    m_add_text(o);
    freeReg(registerState, reg);
}

void m_stop_stmt() {
}

void m_return_stmt() {
}

void m_read_stmt(slist* ls) {
    if (ls == NULL) {
        return;
    }
    m_read_expr(ls->data);
    m_read_stmt(ls->next);
}

void m_write_stmt(slist* ls) {
    if (ls == NULL) {
        return;
    }
    m_write_expr(ls->data);
    m_write_stmt(ls->next);
}

void m_proc_stmt(int funcLabel, slist* paramExprs) {
    // TODO - for funcs, return value space will be pushed first
    // Push all registers
    m_push_all_regs();
    regstate* oldRegState = registerState;
    init_registerState(); // get new registerState;
    // put param values on stack
    m_push_parameter_exprs(paramExprs);
    // jump and link
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "jal %s_%i\n", FUNC_LABEL, funcLabel);
    m_add_text(o);

    // Pop all registers
    m_pop_all_regs();
    free(registerState);
    registerState = oldRegState;
    // TODO - for funcs, deal with return value
}

void m_function_end(int funclabel) {
    // this will add a label to the function end and clear the stack
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s_%i_end:\n", FUNC_LABEL, funclabel);
    m_add_text(o);
    // Set the stack pointer to be the frame pointer - this clears the stack frame
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "move $sp $fp\n");
    m_add_text(o);
    // Return to caller
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "j $ra\n");
    m_add_text(o);
}

void m_move_stack_ptr(int size) {
// Moves the stack pointer, for pushing and popping
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "addi $sp %i\n", size);
    m_add_text(o);
}

void m_set_fp_to_sp(int offsetFromSp) {
// sets the frame pointer before stack pointer (to set it before passed in args)
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "move $fp $sp\n");
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "addi $fp %i\n", offsetFromSp);
    m_add_text(o);
}

void m_push_reg(int reg) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "sw $%i ($sp)\n", reg);
    m_add_text(o);
    // advance sp one word
    m_move_stack_ptr(4);
}

void m_pop_reg(int reg) {
    // move sp back one word
    m_move_stack_ptr(-4);
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "lw $%i ($sp)\n", reg);
    m_add_text(o);
}

void m_push_all_regs() {
    for(int i = 2; i <= 31; ++i) {
        m_push_reg(i);
    }
}

void m_pop_all_regs() {
    for(int i = 31; i >= 2; --i) {
        m_pop_reg(i);
    }
}

void m_push_word_from_reg(int reg) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "sw $%i ($sp)\n", reg);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "addi $sp 4\n");
    m_add_text(o);
}

void m_push_byte_from_reg(int reg) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "sb $%i ($sp)\n", reg);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "addi $sp 1\n");
    m_add_text(o);
}

void m_push_parameter_exprs(slist* paramExprs) {
    while (paramExprs != NULL) {
        expr* e = paramExprs->data;
        if (e->type == int_type) {
            int reg = evalExpr(e);
            m_push_word_from_reg(reg);
            freeReg(registerState, reg);
        } else if (e->type == char_type || e->type == bool_type) {
            int reg = evalExpr(e);
            m_push_byte_from_reg(reg);
            freeReg(registerState, reg);
        } else {
            // TODO - handle user defined types
        }

        paramExprs = paramExprs->next;
    }
}

void m_load_frame_word(int reg, int offset) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "lw $%i %i($fp)\n", reg, offset);
    m_add_text(o);
}
void m_load_frame_byte(int reg, int offset) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "lb $%i %i($fp)\n", reg, offset);
    m_add_text(o);
}

