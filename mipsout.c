
#include "slist.h"
#include "expression.h"

#define CONST_STR_LABEL "m_constStr"
#define BRANCH_LABEL "m_branch"
#define LOAD_CONST_STRLEN 50
#define CONST_STRDEF_STRLEN 50
#define OPERATOR_STRLEN 50

#define SYSC_PRINT_INT 1
#define SYSC_PRINT_STR 4
#define SYSC_READ_INT 5
#define SYSC_READ_STR 8
#define SYSC_EXIT 10


// These are all slists of strings
htslist* m_data;
htslist* m_text;
htslist* m_main;

int strConstIndex = 0;
int branchLabelIndex = 0;

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
            case '\t':
                newstr[nsi++] = '\\';
                newstr[nsi++] = 't';
            case '\"':
                newstr[nsi++] = '\\';
                newstr[nsi++] = '\"';
            default:
                newstr[nsi++] = str[i];

        }
    }
    return newstr;
}

void m_add_string_constant(char* str) {
    m_add_data(CONST_STR_LABEL);
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
        snprintf(o, LOAD_CONST_STRLEN, "lui $%i, %i\n", reg, upper16(n));
        m_add_text(o);
        o = malloc(LOAD_CONST_STRLEN*sizeof(char));
        snprintf(o, LOAD_CONST_STRLEN, "andi $%i, $%i, %i\n", reg, reg, lower16(n));
        m_add_text(o);
    } else if (e->type == char_type) {
        char c = e->char_val;
        o = malloc(LOAD_CONST_STRLEN*sizeof(char));
        snprintf(o, LOAD_CONST_STRLEN, "lui $%i, %i\n", reg, c);
        m_add_text(o);
    } else if (e->type == bool_type) {
        int b = e->bool_val ? 1 : 0;
        o = malloc(LOAD_CONST_STRLEN*sizeof(char));
        snprintf(o, LOAD_CONST_STRLEN, "lui $%i, %i\n", reg, b);
        m_add_text(o);
    } else if (e->type == str_type) {
        // Load the address of the constant string by its index
        int snum = e->str_const_index;
        o = malloc(LOAD_CONST_STRLEN*sizeof(char));
        snprintf(o, LOAD_CONST_STRLEN, "la $%i, %s%i\n", reg, CONST_STR_LABEL, b);
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

void write_expr(expr* e) {
    if (e->type == int_type) {
        m_write_int(evalExpr(e));
    } else if (e->type == str_type) {
        m_write_str(evalExpr(e));
    }
    // TODO - write other data
}

void m_write_str_index(int strIndex) {
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
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "move $a0, $%i\n", reg);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "li $v0, $%i\n", SYSC_PRINT_STR);
    m_add_text(o);
    m_add_text("syscall\n");
}

void m_write_int(int reg) {
    char* o;
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "or $a0, $0, $%i\n", reg);
    m_add_text(o);
    o = malloc(OPERATOR_STRLEN*sizeof(char));
    snprintf(o, OPERATOR_STRLEN, "li $v0, $%i\n", SYSC_PRINT_INT);
    m_add_text(o);
    m_add_text("syscall\n");
}

void m_write_char() {
}

void m_write_bool() {
}

