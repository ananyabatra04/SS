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

#define OPTIONS "b:i:n:d:s:vh"

int main(int argc, char **argv) {
    int opt = 0;
    uint64_t minbits = 256;
    uint64_t iters = 50;
    uint64_t seed = time(NULL);
    int verbose = 0;
    int help = 0;
    char *pbname = "ss.pub";
    char *pvname = "ss.priv";
    char *name;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b': minbits = strtoul(optarg, NULL, 10); break;
        case 'i': iters = strtoul(optarg, NULL, 10); break;
        case 'n': pbname = optarg; break;
        case 'd': pvname = optarg; break;
        case 's': seed = strtoul(optarg, NULL, 10); break;
        case 'v': verbose = 1; break;
        case 'h': help = 1; break;
        }
    }
    if (help) {
        printf("SYNOPSIS\n"
               "    Generates an SS public/private key pair.\n"

               "USAGE\n"
               "    ./keygen [OPTIONS]\n"

               "OPTIONS\n"
               "    -h              Display program help and usage.\n"
               "    -v              Display verbose program output.\n"
               "    -b bits         Minimum bits needed for public key n (default: 256).\n"
               "    -i iterations   Miller-Rabin iterations for testing primes (default: 50).\n"
               "    -n pbfile       Public key file (default: ss.pub).\n"
               "    -d pvfile       Private key file (default: ss.priv).\n"
               "    -s seed         Random seed for testing.\n");

    } else {
        FILE *pbfile = fopen(pbname, "w");
        FILE *pvfile = fopen(pvname, "w");
        if (pbfile == NULL) {
            printf("Can't open public file.\n");
            exit(0);
        }
        if (pvfile == NULL) {
            printf("Can't open private file.\n");
            exit(0);
        }
        fchmod(fileno(pvfile), 0600);

        randstate_init(seed);
        mpz_t p, q, n, d, pq;
        mpz_inits(p, q, n, d, pq, NULL);
        ss_make_pub(p, q, n, minbits, iters);
        ss_make_priv(d, pq, p, q);

        name = getenv("USER");
        ss_write_pub(n, name, pbfile);
        ss_write_priv(pq, d, pvfile);

        if (verbose) {
            printf("user = %s\n", name);
            gmp_printf("p (%ld bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
            gmp_printf("q (%ld bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
            gmp_printf("n (%ld bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
            gmp_printf("pq (%ld bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
            gmp_printf("d (%ld bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
        }

        randstate_clear();
        mpz_clears(p, q, n, d, pq, NULL);
        fclose(pbfile);
        fclose(pvfile);
    }
}
