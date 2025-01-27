#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "randstate.h"
#include "numtheory.h"

// Function returns the lcm between a and b
//
// Credits: I got the implementaion of the
//          Lcm function from Eugenes section
//
void lcm(mpz_t out, mpz_t a, mpz_t b) {

    mpz_t copya, copyb, gcdab, gcdval1;
    mpz_inits(copya, copyb, gcdab, gcdval1, NULL);
    mpz_set(copya, a);
    mpz_set(copyb, b);
    mpz_abs(copya, copya);
    mpz_abs(copyb, copyb);
    mpz_mul(gcdab, copya, copyb);
    gcd(gcdval1, copya, copyb);
    mpz_fdiv_q(gcdval1, gcdab, gcdval1);
    mpz_set(out, gcdval1);
    mpz_clears(copya, copyb, gcdab, gcdval1, NULL);
}

// This function creates a two random prime numbers for
// variables "p" and "q". Variable "p" should use a
// random number of bits within a range and the remaining
// bits will be used to generate variable "p". "p" and "q"
// are multiplied to create the variable "n".
//
// The function aswell creates a suitable public exponent
// by generating a random intger that is nbits long and
// checks if the gcd of the random integer and the
// lambda(n) is equal to 1. That value will then be set
// to the variable "e".
//
// Credits: I got the idea of how to create create a lower
//          upper bound for the random value from Eugenes
//          section.
//          I also got the implementaton of the lambda and
//          lcd fcuntion from Eugenes section
//
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    mpz_t pnum, qnum, nnum, ee, gcdab, gcdval, holder;
    mpz_inits(pnum, qnum, nnum, ee, gcdab, gcdval, holder, NULL);
    mpz_set_ui(pnum, 0);
    mpz_set_ui(qnum, 0);
    mpz_set_ui(gcdab, 0);
    mpz_set_ui(ee, 0);
    mpz_set_ui(gcdval, 0);
    mpz_set_ui(holder, 0);
    mpz_set_ui(nnum, 0);
    uint64_t low = nbits / 4;
    uint64_t up = (3 * nbits) / 4;
    uint64_t pbits = (random() % (up - low + 1)) + low;
    uint64_t qbits = nbits - pbits;
    make_prime(pnum, pbits, iters);
    make_prime(qnum, qbits, iters);
    mpz_mul(nnum, qnum, pnum);
    mpz_set(p, pnum);
    mpz_set(q, qnum);
    mpz_set(n, nnum);
    mpz_sub_ui(pnum, pnum, 1);
    mpz_sub_ui(qnum, qnum, 1);
    lcm(gcdval, pnum, qnum);
    while (mpz_cmp_ui(ee, 1) != 0) {
        mpz_urandomb(holder, state, nbits);
        gcd(ee, holder, gcdval);
    }
    mpz_set(e, holder);
    mpz_clears(pnum, qnum, nnum, ee, gcdab, gcdval, holder, NULL);
    return;
}

// This function writes a public RSA key to pbfile.
//
// Credits: I got the idea of how to use %Zx from
//          Eugenes section
//
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    fprintf(pbfile, "%s\n", username);
}

// This function writes a public RSA key to pbfile.
//
// Credits: I got the idea of how to use %Zx from
//          Eugenes section
//
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n", n);
    gmp_fscanf(pbfile, "%Zx\n", e);
    gmp_fscanf(pbfile, "%Zx\n", s);
    fscanf(pbfile, "%s\n", username);
}

// This function creates a private key which is
// used in later functions.
//
// Credits: Got the idea of how to implement the
//          Lcm and the lambda from Eugenes section
//
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t dnum, ee, pnum, qnum, gcdab, gcdval;
    mpz_inits(dnum, ee, pnum, qnum, gcdab, gcdval, NULL);
    mpz_set_ui(dnum, 0);
    mpz_set_ui(gcdab, 0);
    mpz_set_ui(gcdval, 0);
    mpz_set(pnum, p);
    mpz_set(qnum, q);
    mpz_set(ee, e);
    mpz_sub_ui(pnum, pnum, 1);
    mpz_sub_ui(qnum, qnum, 1);
    lcm(gcdab, pnum, qnum);
    mod_inverse(dnum, ee, gcdab);
    mpz_set(d, dnum);
    mpz_clears(dnum, ee, pnum, qnum, gcdab, gcdval, NULL);
    return;
}

// Writes the mpz variables n and d values into the
// pvfile
//
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

// Scans/reads the mpz variables n and d values into from
// the pvfile
//
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n", n);
    gmp_fscanf(pvfile, "%Zx\n", d);
}

// Encrypts the message m via a pow_mod function
//
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
    return;
}

// This function reads an unencrypted file and parses through it
// block by block and encrypts each block along with adding 0xFF
// hex header indicating a new block. The encrypted message is
// then sent to the outfile
//
// Credits: I got the ideao of feof() from Brian and he aswell helped
//          me understand the syntax needed for this program
//
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    mpz_t c, m;
    mpz_inits(c, m, NULL);
    mpz_set_ui(c, 0);
    mpz_set_ui(m, 0);
    size_t k = mpz_sizeinbase(n, 2);
    size_t j;
    k = (k - 1) / 8;
    uint8_t *block = calloc(k, sizeof(uint8_t));
    block[0] = 0xFF;
    while (!feof(infile)) {
        j = fread(block + 1, sizeof(uint8_t), k - 1, infile);
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, block);
        rsa_encrypt(c, m, e, n);
        gmp_fprintf(outfile, "%Zx\n", c);
    }
    mpz_clears(c, m, NULL);
    free(block);
}

// Function calls the pow_mod function to decrypt the a message
//
// Credits: Based off the pseudocode given in asgn 5 document
//
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
    return;
}

// This function reads an encrypted file and parses through it
// block by block and decrypts it the file and sends the decrypted
// message to the outfile
//
// Credits: I got the ideao of feof() from Brian and he aswell helped
//          me understand the syntax needed for this program
//
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    mpz_t c, m;
    mpz_inits(c, m, NULL);
    mpz_set_ui(c, 0);
    mpz_set_ui(m, 0);
    size_t k = mpz_sizeinbase(n, 2);
    size_t j = 1;
    k = (k - 1) / 8;
    uint8_t *block = calloc(k, sizeof(uint8_t));
    while (!feof(infile)) {
        gmp_fscanf(infile, "%Zx\n", c);
        rsa_decrypt(m, c, d, n);
        mpz_export(&block[0], &j, 1, sizeof(uint8_t), 1, 0, m);
        fwrite(block + 1, sizeof(uint8_t), j - 1, outfile);
    }
    mpz_clears(c, m, NULL);
    free(block);
}

// This function performs an RSA sighning creating a signatue
// that is sent to the variable "s"
//
// Credits: Based off the pseudocode given in asgn 5 document
//
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
    return;
}

// This function performs an RSA verification and returns
// true if "s" is verified if not then it will return false
//
// Credits: Based off the pseudocode given in asgn 5 document
//
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t snum;
    mpz_init(snum);
    mpz_set_ui(snum, 0);
    pow_mod(snum, s, e, n);
    if (mpz_cmp(snum, m) != 0) {
        mpz_clear(snum);
        return false;
    }
    mpz_clear(snum);
    return true;
}
