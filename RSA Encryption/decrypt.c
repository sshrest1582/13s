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
//          decrypt executable from the resources file
//
void usage(char *name) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "    Decrypts data using RSA decryption.\n"
        "    Encrypted data is encrypted by the encrypt program.\n"
        "\n"
        "USAGE\n"
        "      %s [-hv] [-i infile] [-o outfile] -n pubkey -d privkey\n"
        "\n"
        "OPTIONS\n"
        "-h              Display program help and usage.\n"
        "-v              Display verbose program output.\n"
        "-i infile       Input file of data to decrypt (default: stdin).\n"
        "-o outfile      Output file for decrypted data (default: stdout).\n"
        "-d pvfile       Private key file (default: rsa.priv).\n",
        name);
}

// Program reads the private keys from the specified
// private file. The keys are then used to decrypt
// messages within the infile and the decrypted message
// is then sent to the outfile.
//
// Credits: I got majority of this prorgram off the
//          pseudocode on the asgn 5 document.
//          The getopt is also based off the Collatz.c
//          getopt.
int main(int argc, char **argv) {
    bool verbose = false;
    int opt = 0;
    FILE *outfile = stdout;
    FILE *infile = stdin;
    char *pvname = "rsa.priv";
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
                printf("Error opening %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 'n': pvname = optarg; break;
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
    FILE *pvfile = fopen(pvname, "r");
    if (pvfile == NULL) {
        printf("Error opening %s\n", pvname);
        return EXIT_FAILURE;
    }
    mpz_t n, d;
    mpz_inits(n, d, NULL);
    rsa_read_priv(n, d, pvfile);
    if (verbose == true) {
        size_t nbits = mpz_sizeinbase(n, 2);
        size_t dbits = mpz_sizeinbase(d, 2);
        gmp_printf("n (%zu bits) = %Zd\n", nbits, n);
        gmp_printf("d (%zu bits) = %Zd\n", dbits, d);
    }
    rsa_decrypt_file(infile, outfile, n, d);
    fclose(pvfile);
    fclose(outfile);
    fclose(infile);
    mpz_clears(n, d, NULL);
    return 0;
}
