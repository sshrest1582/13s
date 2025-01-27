#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <gmp.h>
#include "randstate.h"

// Creates a global variable
gmp_randstate_t state;

// Initializes the random fuction
// with the desired seed
//
void randstate_init(uint64_t seed) {
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
    srandom(seed);
}

// Clears the state that was used
// randstate_init.
//
void randstate_clear(void) {
    gmp_randclear(state);
}
