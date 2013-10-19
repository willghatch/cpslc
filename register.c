
#include "register.h"


regstate* registerState; // global register state


void clear_regstate(regstate* state) {
    for (int i = 0; i < NUMREG; ++i) {
        state->reg[i] = false;
    }
}

void cp_regstate(regstate* in, regstate* out) {
    for (int i = 0; i < NUMREG; ++i) {
        out->reg[i] = in->reg[i];
    }
}

int getReg(regstate* state) {
    for (int i = 0; i < NUMREG; ++i) {
        if (!state->reg[i]) {
            state->reg[i] = true;
            return i + FIRSTREG;
        }
    }
    // TODO - make this work better.
    return -1;
}

void freeReg(regstate* state, int r) {
    state->reg[r-FIRSTREG] = false;
}

