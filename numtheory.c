#include "numtheory.h"
#include "randstate.h"
#include <gmp.h>

void gcd(mpz_t g, const mpz_t a, const mpz_t b) {
    mpz_t t, temp_a, temp_b;
    mpz_inits(t, temp_a, temp_b, NULL);
    mpz_set(temp_a, a);
    mpz_set(temp_b, b);
    while (mpz_cmp_ui(temp_b, 0) != 0) {
        mpz_set(t, temp_b);
        mpz_mod(temp_b, temp_a, temp_b);
        mpz_set(temp_a, t);
    }
    mpz_set(g, temp_a);
    mpz_clears(t, temp_a, temp_b, NULL);
}

void mod_inverse(mpz_t o, const mpz_t a, const mpz_t n) {
    mpz_t t, t_prime, r, r_prime, q, temp;
    mpz_inits(t, t_prime, r, r_prime, q, temp, NULL);
    mpz_set(r, n);
    mpz_set(r_prime, a);
    mpz_set_ui(t, 0);
    mpz_set_ui(t_prime, 1);
    while (mpz_cmp_ui(r_prime, 0) != 0) {
        mpz_fdiv_q(q, r, r_prime);
        mpz_set(temp, r);
        mpz_set(r, r_prime);
        mpz_submul(temp, q, r_prime);
        mpz_set(r_prime, temp);
        mpz_set(temp, t);
        mpz_set(t, t_prime);
        mpz_submul(temp, q, t_prime);
        mpz_set(t_prime, temp);
    }
    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(o, 0);
        mpz_clears(t, t_prime, r, r_prime, q, temp, NULL);
        return;
    }
    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, n);
    }
    mpz_set(o, t);
    mpz_clears(t, t_prime, r, r_prime, q, temp, NULL);
}

void pow_mod(mpz_t o, const mpz_t a, const mpz_t d, const mpz_t n) {
    mpz_t v, p, temp_d;
    mpz_inits(v, p, temp_d, NULL);
    mpz_set_ui(v, 1);
    mpz_set(p, a);
    mpz_set(temp_d, d);
    while (mpz_cmp_ui(temp_d, 0) > 0) {
        if (mpz_odd_p(temp_d)) {
            mpz_mul(v, v, p);
            mpz_mod(v, v, n);
        }
        mpz_mul(p, p, p);
        mpz_mod(p, p, n);
        mpz_fdiv_q_ui(temp_d, temp_d, 2);
    }
    mpz_set(o, v);
    mpz_clears(v, p, temp_d, NULL);
}

bool is_prime(const mpz_t n, uint64_t iters) {
    if (mpz_cmp_ui(n, 0) == 0) {
        return false;
    } else if (mpz_cmp_ui(n, 1) == 0) {
        return false;
    } else if (mpz_cmp_ui(n, 2) == 0) {
        return true;
    } else if (mpz_cmp_ui(n, 3) == 0) {
        return true;
    }
    mpz_t s, r, i, a, y, temp_n, j, temp_iters, exp, temp2;
    mpz_inits(s, r, i, a, y, temp_n, j, temp_iters, exp, temp2, NULL);
    mpz_set_ui(s, 0);
    mpz_sub_ui(r, n, 1);
    mpz_set_ui(temp_iters, iters);
    mpz_set_ui(exp, 2);
    mpz_sub_ui(temp2, n, 3);
    while (mpz_even_p(r)) { //while r is even
        mpz_fdiv_q_ui(r, r, 2);
        mpz_add_ui(s, s, 1);
    }
    mpz_sub_ui(temp_n, n, 1);
    for (mpz_set_ui(i, 1); mpz_cmp(i, temp_iters) < 0; mpz_add_ui(i, i, 1)) {
        //choose a random value a between 2 and n-2
        mpz_urandomm(a, state, temp2); //set state
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, r, n);
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, temp_n) != 0) {
            mpz_set_ui(j, 1);
            while (mpz_cmp(j, s) < 0 && mpz_cmp(y, temp_n) != 0) {
                pow_mod(y, y, exp, n);
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(s, r, i, a, y, temp_n, j, temp_iters, exp, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            if (mpz_cmp(y, temp_n) != 0) {
                mpz_clears(s, r, i, a, y, temp_n, j, temp_iters, exp, temp2, NULL);
                return false;
            }
        }
    }
    mpz_clears(s, r, i, a, y, temp_n, j, temp_iters, exp, NULL);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_t num;
    mp_bitcnt_t nbits = bits;
    mpz_inits(num, NULL);
    mpz_set_ui(num, 1);
    do {
        mpz_mul_2exp(p, num, nbits);
        mpz_urandomm(p, state, p);
        mpz_add(p, p, p);
        mpz_add_ui(p, p, 1);
    } while (!is_prime(p, iters));
    mpz_clears(num, NULL);
}
