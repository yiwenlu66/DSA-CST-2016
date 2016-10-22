#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define BUF_SZ 1 << 23

char ibuf[BUF_SZ], obuf[BUF_SZ];

int searchH(int e, int* a, int lo, int hi); // a is DESCENDING, find the index of the last element in a[lo, hi) no less than i, hi on failure
int searchL(int e, int* a, int lo, int hi); // a is DESCENDING, find the index of the first element in a[lo, hi) no more than i, lo - 1 on failure

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);

    int N, H, M, h;
    int * pos, * neg;
    double Phit, Pfalse;
    char sign[2];

    // read in +/- distribution
    scanf("%d%d", &N, &H);
    pos = (int*)malloc((H + 1) * sizeof(int));
    neg = (int*)malloc((H + 1) * sizeof(int));
    while (N-- > 0) {
        scanf("%d%s", &h, sign);
        switch (sign[0]) {
        case '+':
            ++pos[h];
            break;
        case '-':
            ++neg[h];
            break;
        default:
            break;
        }
    }

    // calculate cumulative values
    int possum = 0, negsum = 0;
    int i;
    for (i = H; i >= 0; --i) {
        pos[i] = (possum += pos[i]);
        neg[i] = (negsum += neg[i]);
    }

    // make judgements
    int minhit, maxfalse, hL, hH;
    scanf("%d", &M);
    while (M-- > 0) {
        scanf("%lf%lf", &Phit, &Pfalse);
        minhit = (int)ceil(Phit * possum);
        maxfalse = (int)floor(Pfalse * negsum);
        hH = searchH(minhit, pos, 0, H + 1);
        hL = searchL(maxfalse, neg, 0, H + 1);
        if (hL <= hH) {
            printf("%d %d\n", hL, hH);
        } else {
            printf("-1\n");
        }
    }

    free(pos);
    free(neg);

    return 0;
}

int searchL(int e, int* a, int lo, int hi)
{
    while (lo < hi) {
        int mi = lo + ((hi - lo) >> 1);
        if (e >= a[mi]) {
            hi = mi;
        } else {
            lo = mi + 1;
        }
    }
    return lo;
}

int searchH(int e, int* a, int lo, int hi)
{
    while (lo < hi) {
        int mi = lo + ((hi - lo) >> 1);
        if (e > a[mi]) {
            hi = mi;
        } else {
            lo = mi + 1;
        }
    }
    return lo - 1;
}
