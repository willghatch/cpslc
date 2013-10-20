// statement.c

#include "mipsout.h"
#include "register.h"

void read_expr_int(ID* intvar) {
    //read syscall
    //store
    int tr1, tr2;
    tr1 = getReg(registerState);
    tr2 = getReg(registerState);
    m_read_int(tr1);
    if (isGlobal(intvar)) {
        m_assign_int_global(tr1, tr2, intvar->id_label);
    } else {
    // TODO - implement!
    }

    freeReg(registerState, tr1);
    freeReg(registerState, tr2);
}
