#include "stdio.h"
#include "string.h"
#include "stdint.h"
#define BUF_SZ  1 << 23
#define MAXLEN  5000
#define SEG_SZ  8
#define SEG_EXP 100000000UL
#define MAXSEG  1250

const uint64_t EXP[] = { 1UL, 10UL, 100UL, 1000UL, 10000UL, 100000UL, 1000000UL, 10000000UL, 100000000UL, 1000000000UL };

char ibuf[BUF_SZ], obuf[BUF_SZ];
char nbuf[MAXLEN];
uint64_t a[MAXSEG], b[MAXSEG], product[MAXSEG];

size_t read(uint64_t* dst);  // read a number from stdin into dst, return length of number
size_t mul(uint64_t* a, size_t la, uint64_t* b, size_t lb, uint64_t* dst);   // multiply a and b into dst, return length of product
void write(uint64_t* p, size_t lp); // print the result to stdout
int iszero(uint64_t* n, size_t l);

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
        if (iszero(a, la) || iszero(b, lb)) {
            // deal with edge case
            printf("0\n");
        } else {
            lp = mul(a, la, b, lb, product);
            write(product, lp);
        }
    }
    return 0;
}

size_t read(uint64_t* dst)
{
    scanf("%s", nbuf);
    size_t pos = 0;
    int i, j = 0;
    dst[0] = 0;

    // store as little-endian
    for (i = (int)strlen(nbuf) - 1; i >= 0; --i) {
        if (j >= SEG_SZ) {
            j = 0;
            dst[++pos] = 0;
        }
        dst[pos] += EXP[j++] * (nbuf[i] - '0');
    }

    return pos + 1;
}

size_t mul(uint64_t* a, size_t la, uint64_t* b, size_t lb, uint64_t* dst)
{
    memset(dst, 0, sizeof(uint64_t) * MAXSEG);
    size_t i, j;

    // convolution
    for (i = 0; i < la; ++i) {
        for (j = 0; j < lb; ++j) {
            dst[i + j] += a[i] * b[j];
        }
    }

    // carry
    for (i = 0; i < la + lb; ++i) {
        dst[i + 1] += dst[i] / SEG_EXP;
        dst[i] %= SEG_EXP;
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

int iszero(uint64_t* n, size_t l)
{
    return l == 1 && n[0] == 0;
}
