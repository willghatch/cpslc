
#include "mipsout.h"
#include "slist.h"
#include "expression.h"
#include "register.h"
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

void m_writeExpressionList(slist* ls) {
    if (ls == NULL) {
        return;
    }
    m_write_expr(ls->data);
    m_writeExpressionList(ls->next);
}

void m_add_main_label() {
    m_add_text("\nmain:\n");
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
    slist* ls = m_data->head;
    while(ls != NULL) {
        fprintf(f, ls->data);
        ls = ls->next;
    }

    // write text section
    fprintf(f, "\n.text\n");
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

void m_load_global_int(int index, int reg) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "lw $%i, %s%i #load global\n", reg, GLOBAL_VAR_LABEL, index);
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

void m_assign_int_global(int reg, int globalIndex) {
    m_add_text("#storing global\n");
    char* o;
    //o = malloc(OPERATOR_STRLEN*sizeof(char));
    //snprintf(o, OPERATOR_STRLEN, "la $%i, %s%i\n", tempreg, GLOBAL_VAR_LABEL, globalIndex);
    //m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "sw $%i, %s%i\n", reg, GLOBAL_VAR_LABEL, globalIndex);
    // TODO - allow an offset input.
    m_add_text(o);
}

void m_read_expr_int(ID* intvar) {
    //read syscall
    //store
    int reg;
    reg = getReg(registerState);
    m_read_int(reg);
    if (isGlobal(intvar)) {
        m_assign_int_global(reg, intvar->id_label);
    } else {
    // TODO - implement!
    }

    freeReg(registerState, reg);
}

void m_read_expr(expr* e) {
    if (e->kind == globalVar) {
        m_read_expr_int(e->edata.globalId);
    }
    // TODO - implement for other expressions than just global vars...
}

void m_readExpressionList(slist* ls) {
    if (ls == NULL) {
        return;
    }
    m_read_expr(ls->data);
    m_readExpressionList(ls->next);
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
    snprintf(o, OPERATOR_STRLEN, "%s%iA:\n", opstr, r_dest, BRANCH_LABEL, bi);
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
    snprintf(o, OPERATOR_STRLEN, "%s%iB:\n", opstr, r_dest, BRANCH_LABEL, bi);
    m_add_text(o);
    // li <dest> 0 # false case
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "li $%i 0\n", r_dest);
    m_add_text(o);
    // LABELXC:
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "%s%iC:\n", opstr, r_dest, BRANCH_LABEL, bi);
    m_add_text(o);
    // And now we continue with the operator return value in <dest>
}


