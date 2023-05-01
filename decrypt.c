#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "randstate.h"
#include "numtheory.h"
#include "ss.h"
#include <gmp.h>

#define OPTIONS "i:o:n:vh"

int main(int argc, char **argv) {
    int opt = 0;
    int verbose = 0;
    int help = 0;
    FILE *infile = stdin;
    FILE *outfile = stdout;
    char *pvname = "ss.priv";
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            infile = fopen(optarg, "r");
            if (infile == NULL) {
                printf("Can't open file.\n");
                exit(0);
            }
            break;
        case 'o':
            outfile = fopen(optarg, "w");
            if (outfile == NULL) {
                printf("Can't open file.\n");
                exit(0);
            }
            break;
        case 'n': pvname = optarg; break;
        case 'v': verbose = 1; break;
        case 'h': help = 1; break;
        }
    }
    if (help) {
        printf("SYNOPSIS\n"
               "    Encrypts data using SS encryption.\n"
               "    Encrypted data is decrypted by the decrypt program.\n"

               "USAGE\n"
               "    ./encrypt [OPTIONS]\n"

               "OPTIONS\n"
               "    -h              Display program help and usage.\n"
               "    -v              Display verbose program output. \n"
               "    -i infile       Input file of data to encrypt (default: stdin).\n"
               "    -o outfile      Output file for encrypted data (default: stdout).\n"
               "    -n pvfile       Private key file (default: ss.priv).\n"

        );
    } else {
        mpz_t pq, d;
        mpz_inits(pq, d);
        FILE *pvfile = fopen(pvname, "r");
        if (pvfile == NULL) {
            printf("Can't open public file.\n");
            exit(0);
        }
        ss_read_priv(pq, d, pvfile);
        if (verbose) {
            gmp_printf("pq (%ld bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
            gmp_printf("d (%ld bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
        }
        ss_decrypt_file(infile, outfile, d, pq);
        fclose(pvfile);
        mpz_clears(pq, d);
    }
    fclose(infile);
    fclose(outfile);
}
