
#include "register.h"
#include <stdlib.h>


regstate* registerState; // global register state

void init_registerState() {
    registerState = malloc(sizeof(regstate));
    clear_regstate(registerState);
}

void clear_regstate(regstate* state) {
    for (int i = 0; i < NUMREG; ++i) {
        state->reg[i] = 0;
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
            state->reg[i] = 1;
            return i + FIRSTREG;
        }
    }
    // TODO - make this work better.
    return -1;
}

void freeReg(regstate* state, int r) {
    state->reg[r-FIRSTREG] = 0;
}

