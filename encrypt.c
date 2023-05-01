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
    char *pbname = "ss.pub";
    char name[1024] = { 0 };

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
        case 'n': pbname = optarg; break;
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
               "    -n pbfile       Public key file (default: ss.pub).\n"

        );
    } else {
        mpz_t n;
        mpz_init(n);
        FILE *pbfile = fopen(pbname, "r");
        if (pbfile == NULL) {
            printf("Can't open public file.\n");
            exit(0);
        }
        ss_read_pub(n, name, pbfile);
        if (verbose) {
            printf("user = %s\n", name);
            gmp_printf("n (%ld bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        }
        ss_encrypt_file(infile, outfile, n);
        mpz_clear(n);
        fclose(pbfile);
    }
    fclose(infile);
    fclose(outfile);
}
