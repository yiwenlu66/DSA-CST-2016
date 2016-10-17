#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "math.h"
#define BUF_SZ  1 << 23
#define MAXLEN  10000
#define SEG_SZ  9
#define SEG_EXP 1000000000UL

char ibuf[BUF_SZ], obuf[BUF_SZ];
char nbuf[MAXLEN];
uint64_t a[MAXLEN], b[MAXLEN], product[MAXLEN];

size_t read(uint64_t* dst);  // return length of number
size_t mul(uint64_t* a, size_t la, uint64_t* b, size_t lb, uint64_t* dst);   // return length of product
void write(uint64_t* p, size_t lp);

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);
    int n;
    size_t la, lb, lp;
    scanf("%d", &n);
    while (n-- > 0) {
        la = read(a);
        lb = read(b);
        lp = mul(a, la, b, lb, product);
        write(product, lp);
    }
    return 0;
}

size_t read(uint64_t* dst)
{
    scanf("%s", nbuf);
    size_t l = strlen(nbuf);
    size_t sz = (size_t)(ceil(1.0 * l / SEG_SZ));
    size_t i, j;
    uint64_t exp;
    for (i = 0; i < sz; ++i) {
        dst[i] = 0;
        for (j = 0, exp = 1; j < SEG_SZ && SEG_SZ * i + j < l; ++j, exp *= 10) {
            dst[i] += exp * (nbuf[l - 1 - i * SEG_SZ - j] - '0');  // little-endian
        }
    }
    return sz;
}

size_t mul(uint64_t* a, size_t la, uint64_t* b, size_t lb, uint64_t* dst)
{
    memset(dst, 0, sizeof(uint64_t) * MAXLEN);
    uint64_t carry = 0;
    size_t i, j;
    for (i = 0; i < la; ++i) {
        for (j = 0; j < lb; ++j) {
            uint64_t digit_with_carry = a[i] * b[j] + dst[i + j] + carry;
            dst[i + j] = digit_with_carry % SEG_EXP;
            carry = digit_with_carry / SEG_EXP;
        }
        dst[i + lb] = carry;
        carry = 0;
    }
    return dst[la + lb - 1] ? (la + lb) : (la + lb - 1);
}

void write(uint64_t* p, size_t lp)
{
    size_t i;
    printf("%lu", p[lp - 1]);
    for (i = 1; i < lp; ++i) {
        printf("%0*lu", SEG_SZ, p[lp - i - 1]);
    }
    printf("\n");
}
