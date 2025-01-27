#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

// Usage page that directs the user on how to properly
// use the program
//
// Credits: Based of the Collaztz.c usage page. The
//          acutal Synopsis and etc is from the
//          keygen executable from the resources file
//
void usage(char *name) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "Generates an RSA public/private key pair.\n"
        "\n"
        "USAGE\n"
        "%s [-hv] [-b bits] -n pbfile -d pvfile\n"
        "\n"
        "OPTIONS\n"
        "-h              Display program help and usage.\n"
        "-v              Display verbose program output.i\n"
        "-b bits         Minimum bits needed for public key n.\n"
        "-c confidence   Miller-Rabin iterations for testing primes (default: 50).\n"
        "-n pbfile       Public key file (default: rsa.pub).\n"
        "-d pvfile       Private key file (default: rsa.priv).\n"
        "-s seed         Random seed for testing.\n",
        name);
}

// Program that creates the public and private keys
// which is then sent to the respective files. It
// aswell creates the signature from the users username
// the user is able to decide which files the public and
// private keys are sent. The user can also decide if
// they want to see the bit lengths and values of the
// public key, private key, and signature
//
// Credits: I got the idea of how to use the getopt from
//          Collatz.c
//          Majority of this code is based of the pseudo
//          code given from the asgn 5 document
//
int main(int argc, char **argv) {
    uint64_t seed = time(NULL);
    srandom(seed);
    int64_t pvnumber;
    char *user = getenv("USER");
    uint64_t bits = 256;
    uint64_t confidence = 50;
    bool verbose = false;
    int opt = 0;
    char *pvstring = "rsa.priv";
    char *pubstring = "rsa.pub";
    while ((opt = getopt(argc, argv, "h, v, c:, b:, n:, d:, s:")) != -1) {
        switch (opt) {
        case 'h':
            usage(argv[0]);
            return EXIT_FAILURE;
            break;
        case 'v': verbose = true; break;
        case 'b': bits = (uint64_t) strtoul(optarg, NULL, 10); break;
        case 'c': confidence = (uint64_t) strtoul(optarg, NULL, 10); break;
        case 'n': pubstring = optarg; break;
        case 'd': pvstring = optarg; break;
        case 's': seed = (uint64_t) strtoul(optarg, NULL, 10); break;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }
    FILE *pubfile = fopen(pubstring, "w");
    if (pubfile == NULL) {
        printf("Error opening %s\n", pubstring);
        return EXIT_FAILURE;
    }
    FILE *pvfile = fopen(pvstring, "w");
    if (pvfile == NULL) {
        printf("Error opening %s\n", pvstring);
        return EXIT_FAILURE;
    }
    mpz_t n, d, e, q, p, s, m;
    mpz_inits(n, d, e, q, p, s, m, NULL);
    randstate_init(seed);
    pvnumber = fileno(pvfile);
    fchmod(pvnumber, 0600);
    rsa_make_pub(p, q, n, e, bits, confidence);
    rsa_make_priv(d, e, p, q);
    mpz_set_str(m, user, 62);
    rsa_sign(s, m, d, n);
    rsa_write_pub(n, e, s, user, pubfile);
    rsa_write_priv(n, d, pvfile);
    if (verbose == true) {
        size_t pbits = mpz_sizeinbase(p, 2);
        size_t qbits = mpz_sizeinbase(q, 2);
        size_t nbits = mpz_sizeinbase(n, 2);
        size_t dbits = mpz_sizeinbase(d, 2);
        size_t sbits = mpz_sizeinbase(s, 2);
        size_t ebits = mpz_sizeinbase(e, 2);
        printf("user = %s\n", user);
        gmp_printf("s (%zu bits) = %Zd\n", sbits, s);
        gmp_printf("p (%zu bits) = %Zd\n", pbits, p);
        gmp_printf("q (%zu bits) = %Zd\n", qbits, q);
        gmp_printf("n (%zu bits) = %Zd\n", nbits, n);
        gmp_printf("e (%zu bits) = %Zd\n", ebits, e);
        gmp_printf("d (%zu bits) = %Zd\n", dbits, d);
    }
    fclose(pvfile);
    fclose(pubfile);
    randstate_clear();
    mpz_clears(n, d, e, q, p, s, m, NULL);
    return 0;
}
