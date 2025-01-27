#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "randstate.h"
#include "numtheory.h"

// Function funcd the gcd between variables a
// and b. That values is then sent to variable d.
//
// Credits: I got the implementation of the gcd
//          function from the pseudo code privided
//          in asgn 5 document
//
void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t aa, bb, t;
    mpz_inits(aa, bb, t, NULL);
    mpz_set_ui(t, 0);
    mpz_set(aa, a);
    mpz_set(bb, b);
    while (mpz_cmp_ui(bb, 0) != 0) {
        mpz_set(t, bb);
        mpz_mod(bb, aa, bb);
        mpz_set(aa, t);
    }
    mpz_set(d, aa);
    mpz_clears(aa, bb, t, NULL);
}

// This function find the modular inverse of
// the variables a and n. Said variable is
// sent to variable i
//
// Credits: I got the implementation of the
//          mod_inverse function from the
//          provided pseudocode from the
//          asgn 5 docuement
//
void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    mpz_t ra, rn, tzero, tone, q, holder, div;
    mpz_inits(ra, rn, tzero, tone, q, holder, div, NULL);
    mpz_set(ra, a);
    mpz_set(rn, n);
    mpz_set_ui(q, 0);
    mpz_set_ui(holder, 0);
    mpz_set_ui(div, 0);
    mpz_set_ui(tzero, 0);
    mpz_set_ui(tone, 1);
    while (mpz_cmp_ui(ra, 0) != 0) {
        mpz_fdiv_q(div, rn, ra);
        mpz_set(q, div);
        mpz_set(holder, rn);
        mpz_set(rn, ra);
        mpz_submul(holder, q, ra);
        mpz_set(ra, holder);
        mpz_set(holder, tzero);
        mpz_set(tzero, tone);
        mpz_submul(holder, q, tone);
        mpz_set(tone, holder);
    }
    if (mpz_cmp_ui(rn, 1) > 0) {
        mpz_set_ui(i, 0);
        mpz_clears(ra, rn, tzero, tone, q, holder, div, NULL);
        return;
    }
    if (mpz_cmp_ui(tzero, 0) < 0) {
        mpz_add(tzero, tzero, n);
    }
    mpz_set(i, tzero);
    mpz_clears(ra, rn, tzero, tone, q, holder, div, NULL);
    return;
}

// Function calculates the base to the power of the
// exponent modular moldulus and send the value to
// out. The function mimics the mpz_powm function
//
// Credtis: I got the idea of the implementation
//          for the pow_mod function from the
//          provided pseudo code in asgn 5
//
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    mpz_t v, bass, expo, mode;
    mpz_inits(v, bass, expo, mode, NULL);
    mpz_set_ui(v, 1);
    mpz_set(bass, base);
    mpz_set(expo, exponent);
    mpz_set(mode, modulus);
    while (mpz_cmp_ui(expo, 0) > 0) {
        if (mpz_odd_p(expo) != 0) {
            mpz_mul(v, v, bass);
            mpz_mod(v, v, mode);
        }
        mpz_mul(bass, bass, bass);
        mpz_mod(bass, bass, mode);
        mpz_fdiv_q_ui(expo, expo, 2);
    }
    mpz_set(out, v);
    mpz_clears(v, bass, expo, mode, NULL);
    return;
}

// Boolean fcuntion that returns whether or not
// the values of n is a prime number or not.
//
// Credits: I got the implementation of the
//          is_prime function from the
//          provided pseudocode in asgn 5
//          document
//
bool is_prime(mpz_t n, uint64_t iters) {
    mpz_t i, r, s, k, copyn, nthree, ntwo, none, y, a, j, comp;
    mpz_inits(i, r, s, k, copyn, nthree, none, ntwo, y, a, j, comp, NULL);
    mpz_set(nthree, n);
    mpz_set(ntwo, n);
    mpz_set(none, n);
    mpz_set(copyn, n);
    mpz_set_ui(k, iters);
    mpz_set_ui(i, 0);
    mpz_set_ui(r, 0);
    mpz_set_ui(s, 0);
    mpz_set_ui(j, 0);
    mpz_set_ui(a, 0);
    mpz_set_ui(y, 0);
    mpz_set_ui(comp, 2);
    mpz_sub_ui(nthree, nthree, 3);
    mpz_sub_ui(none, none, 1);
    mpz_sub_ui(ntwo, ntwo, 2);
    mpz_sub_ui(r, copyn, 1);
    if (mpz_cmp_ui(n, 0) == 0 || mpz_cmp_ui(n, 1) == 0 || mpz_cmp_ui(n, 2) == 0
        || mpz_cmp_ui(n, 3) == 0) {
        mpz_clears(i, r, s, k, copyn, nthree, none, ntwo, y, a, j, comp, NULL);
        return false;
    }
    while (mpz_even_p(r) != 0) {
        mpz_add_ui(s, s, 1);
        mpz_fdiv_q(r, r, comp);
    }
    mpz_sub_ui(s, s, 1);
    while (mpz_cmp(i, k) < 0) {
        mpz_urandomm(a, state, nthree);
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, r, copyn);
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, none) != 0) {
            mpz_set_ui(j, 1);
            while (mpz_cmp(j, s) <= 0 && mpz_cmp(y, none) != 0) {
                pow_mod(y, y, comp, n);
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(i, r, s, k, copyn, nthree, none, ntwo, y, a, j, comp, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            if (mpz_cmp(y, none) != 0) {
                mpz_clears(i, r, s, k, copyn, nthree, none, ntwo, y, a, j, comp, NULL);
                return false;
            }
        }
        mpz_add_ui(i, i, 1);
    }
    mpz_clears(i, r, s, k, copyn, nthree, none, ntwo, y, a, j, comp, NULL);
    return true;
}

// Function creates a prime number that has a bit lenfth
// above the value of bits.
// Credits: I got the implementation of the
//          make_prime function from the
//          provided pseudocode in asgn 5
//          document
//
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_t two, num, byte;
    mpz_inits(two, num, byte, NULL);
    mpz_set_ui(two, 2);
    mpz_set_ui(num, 0);
    mpz_set_ui(byte, 0);
    mpz_pow_ui(byte, two, bits);
    bool barrier = false;
    while (barrier == false) {
        mpz_urandomb(num, state, bits);
        mpz_add(num, num, byte);
        if (is_prime(num, iters) == true) {
            barrier = true;
        }
    }
    mpz_set(p, num);
    mpz_clears(two, num, byte, NULL);
    return;
}
