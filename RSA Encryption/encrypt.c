#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

// Usage page that directs the user on how to properly
// use the program
//
// Credits: Based of the Collaztz.c usage page. The
//          acutal Synopsis and etc is from the
//          encrypt executable from the resources file
//
void usage(char *name) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   Encrypts data using RSA encryption.\n"
        "   Encrypted data is decrypted by the decrypt program.\n"
        "\n"
        "   USAGE\n"
        "       %s [-hv] [-i infile] [-o outfile] -n pubkey -d privkey\n"
        "\n"
        "   OPTIONS\n"
        "     -h              Display program help and usage.\n"
        "     -v              Display verbose program output.\n"
        "     -i infile       Input file of data to encrypt (default: stdin).\n"
        "     -o outfile      Output file for encrypted data (default: stdout).\n"
        "     -n pbfile       Public key file (default: rsa.pub).\n",
        name);
}

// Program reads the public key files and encrypts the
// messages within the infile and outputs it to the
// outfile while making sure the rsa signature is
// verified. The user is able to view the bit length
// of the value s, n, e and the username. The user can
// also specifiy the infile, outfile, and the public
// key file if wanted.
//
// Credits: Majority of this program is based off the
//          pseudocode provided in asgn 5 document
//          I also go the idea of doing user[256]
//          from Omar
//
int main(int argc, char **argv) {
    bool verbose = false;
    int opt = 0;
    char user[256];
    FILE *outfile = stdout;
    FILE *infile = stdin;
    char *pubman = "rsa.pub";
    while ((opt = getopt(argc, argv, "h, v, i:, o:, n:")) != -1) {
        switch (opt) {
        case 'h':
            usage(argv[0]);
            return EXIT_FAILURE;
            break;
        case 'v': verbose = true; break;
        case 'i':
            infile = fopen(optarg, "r");
            if (infile == NULL) {
                printf("Error opening %s\n", pubman);
                return EXIT_FAILURE;
            }
            break;
        case 'n': pubman = optarg; break;
        case 'o':
            outfile = fopen(optarg, "w");
            if (outfile == NULL) {
                printf("Error opening %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }
    FILE *pubfile = fopen(pubman, "r");
    if (pubfile == NULL) {
        printf("Error opening %s\n", pubman);
        return EXIT_FAILURE;
    }
    mpz_t n, e, s, d, m;
    mpz_inits(n, m, e, s, d, NULL);
    rsa_read_pub(n, e, s, user, pubfile);
    if (verbose == true) {
        size_t nbits = mpz_sizeinbase(n, 2);
        size_t sbits = mpz_sizeinbase(s, 2);
        size_t ebits = mpz_sizeinbase(e, 2);
        printf("user = %s\n", user);
        gmp_printf("s (%zu bits) = %Zd\n", sbits, s);
        gmp_printf("n (%zu bits) = %Zd\n", nbits, n);
        gmp_printf("e (%zu bits) = %Zd\n", ebits, e);
    }
    mpz_set_str(m, user, 62);
    if (rsa_verify(m, s, e, n) == false) {
        printf("Error signature could not be verified\n");
        return EXIT_FAILURE;
    }
    rsa_encrypt_file(infile, outfile, n, e);
    mpz_clears(n, m, e, s, d, NULL);
    fclose(infile);
    fclose(outfile);
    fclose(pubfile);
    return 0;
}
