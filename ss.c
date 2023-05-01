#include <stdlib.h>
#include "ss.h"
#include "numtheory.h"
#include "randstate.h"
#include <gmp.h>

// Generates the components for a new SS key.
//
// Provides:
//  p:  first prime
//  q: second prime
//  n: public modulus/exponent
//
// Requires:
//  nbits: minimum # of bits in n
//  iters: iterations of Miller-Rabin to use for primality check
//  all mpz_t arguments to be initialized
//
void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    mpz_t temp_q, temp_p;
    mpz_inits(temp_q, temp_p, NULL);
    uint64_t n_bits = nbits / 5 + 1;
    uint64_t pbits = random() % n_bits;
    pbits += n_bits;
    uint64_t qbits = nbits - 2 * pbits;

    make_prime(p, pbits, iters);
    mpz_sub_ui(temp_p, p, 1);
    do {
        make_prime(q, qbits, iters);
        mpz_sub_ui(temp_q, q, 1);

    } while ((mpz_cmp(p, q) == 0) || mpz_divisible_p(p, temp_q) || mpz_divisible_p(q, temp_p));
    //returns non zero if numerator is exactly divisible by denominator
    mpz_mul(n, p, p);
    mpz_mul(n, n, q);
    mpz_clears(temp_q, temp_p, NULL);
}

//
// Generates components for a new SS private key.
//
// Provides:
//  d:  private exponent
//  pq: private modulus
//
// Requires:
//  p:  first prime number
//  q: second prime number
//  all mpz_t arguments to be initialized
//
void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {
    mpz_mul(pq, p, q);
    mpz_t gcd_val, temp_p, temp_q, totient, n;
    mpz_inits(gcd_val, temp_p, temp_q, totient, n, NULL);
    mpz_sub_ui(temp_p, p, 1);
    mpz_sub_ui(temp_q, q, 1);
    mpz_mul(totient, temp_p, temp_q);
    gcd(gcd_val, temp_p, temp_q);
    mpz_fdiv_q(totient, totient, gcd_val);
    mpz_mul(n, p, p);
    mpz_mul(n, n, q);
    mpz_set(d, totient);
    mod_inverse(d, n, totient);

    mpz_clears(gcd_val, temp_p, temp_q, totient, n, NULL);
}
//
// Export SS public key to output stream
//
// Requires:
//  n: public modulus/exponent
//  username: login name of keyholder ($USER)
//  pbfile: open and writable file stream
//
void ss_write_pub(const mpz_t n, const char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    fprintf(pbfile, "%s\n", username);
}
//
// Export SS private key to output stream
//
// Requires:
//  pq: private modulus
//  d:  private exponent
//  pvfile: open and writable file stream
//

void ss_write_priv(const mpz_t pq, const mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", pq);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

//
// Import SS public key from input stream
//
// Provides:
//  n: public modulus
//  username: $USER of the pubkey creator
//
// Requires:
//  pbfile: open and readable file stream
//  username: requires sufficient space
//  all mpz_t arguments to be initialized
//
void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx", n);
    fscanf(pbfile, "%s", username);
}
//
// Import SS private key from input stream
//
// Provides:
//  pq: private modulus
//  d:  private exponent
//
// Requires:
//  pvfile: open and readable file stream
//  all mpz_t arguments to be initialized
//

void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx", pq);
    gmp_fscanf(pvfile, "%Zx", d);
}

//
// Encrypt number m into number c
//
// Provides:
//  c: encrypted integer
//
// Requires:
//  m: original integer
//  n: public exponent/modulus
//  all mpz_t arguments to be initialized
//
void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {
    pow_mod(c, m, n, n);
}

//
// Encrypt an arbitrary file
//
// Provides:
//  fills outfile with the encrypted contents of infile
//
// Requires:
//  infile: open and readable file stream
//  outfile: open and writable file stream
//  n: public exponent and modulus
//
void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {
    mpz_t m, c;
    mpz_inits(m, c, NULL);
    uint64_t k = (mpz_sizeinbase(n, 2) - 1) / 2;
    k = (k - 1) / 8;
    size_t num = 0;
    uint8_t *array = (uint8_t *) calloc(k, sizeof(uint8_t));
    array[0] = 255; //0xFF

    while ((num = fgetc(infile)) != EOF) {
        uint64_t j = 1;
        do {
            array[j] = num;
            j++;
        } while ((j < k) && (num = fgetc(infile)) != -1);
        mpz_import(m, j, 1, sizeof(uint8_t), 1, 0, array);
        ss_encrypt(c, m, n);
        gmp_fprintf(outfile, "%Zx\n", c);
    }

    mpz_clears(m, c, NULL);
    free(array);
}

//
// Decrypt number c into number m
//
// Provides:
//  m: decrypted/original integer
//
// Requires:
//  c: encrypted integer
//  d: private exponent
//  pq: private modulus
//  all mpz_t arguments to be initialized
//

void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {
    pow_mod(m, c, d, pq);
}

//
// Decrypt a file back into its original form.
//
// Provides:
//  fills outfile with the unencrypted data from infile
//
// Requires:
//  infile: open and readable file stream to encrypted data
//  outfile: open and writable file stream
//  d: private exponent
//  pq: private modulus

void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {
    mpz_t c, m;
    size_t j;
    mpz_inits(c, m, NULL);
    uint64_t k = (mpz_sizeinbase(pq, 2) - 1) / 2;
    k = (k - 1) / 8;

    uint8_t *array = (uint8_t *) calloc(k, sizeof(uint8_t));

    while (gmp_fscanf(infile, "%Zx", c) != EOF) {
        ss_decrypt(m, c, d, pq);
        mpz_export(array, &j, 1, sizeof(uint8_t), 1, 0, m);
        fwrite(array + 1, sizeof(uint8_t), j - 1, outfile);
    }
    mpz_clears(c, m, NULL);
    free(array);
}
