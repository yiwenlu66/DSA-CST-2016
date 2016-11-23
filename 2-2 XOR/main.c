#include <stdio.h>
#include <stdint.h>
#define BUF_SZ (1 << 20)
#define MAXN 500000
#define MOD 2147483648

char ibuf[BUF_SZ], obuf[BUF_SZ];
int A[MAXN];

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);

    int N, K;
    scanf("%d%d", &N, &K);
    int i;
    for (i = 0; i < N; ++i) {
        scanf("%d", &A[i]);
    }

    uint64_t E = 0;
    for (i = 0; i < N; ++i) {
        int Ei = A[i], maxEi = A[i];
        int k;
        for (k = 1; k < K && i + k < N; ++k) {
            Ei ^= A[i + k];
            if (Ei > maxEi) {
                maxEi = Ei;
            }
        }
        E = (E + maxEi) % MOD;
    }
    printf("%lld\n", E);
    return 0;
}
