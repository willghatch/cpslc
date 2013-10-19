
#ifndef REGISTER_H
#define REGISTER_H


#define NUMREG 17
#define FIRSTREG 8
// I'll use registers 8-25 as general purpose

typedef struct regstate_struct regstate;
struct regstate_struct {
    int reg[NUMREG]; // int as bool
};

extern regstate* registerState;

void clear_regstate(regstate* state);
void cp_regstate(regstate* in, regstate* out);
int getReg(regstate* state);
void freeReg(regstate* state, int r);
void init_registerState();


#endif //REGISTER_H
