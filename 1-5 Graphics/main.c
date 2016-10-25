#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define IBUF_SZ 1 << 23
#define OBUF_SZ 1 << 21
#define MAXN 200000

char ibuf[IBUF_SZ], obuf[OBUF_SZ];
int x[MAXN], y[MAXN];

int cmp(const void* p1, const void* p2);
int search(int Px, int Py, int n);    // return the index of the last segment to the left of or passing point (Px, Py)
int toleft(int Px, int Py, int x, int y);   // whether point (Px, Py) is to the left of the line connecting (x, 0) and (0, y)
int64_t cross(int x1, int y1, int x2, int y2);  // z component of (x1, y1, 0) x (x2, y2, 0)

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, IBUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, OBUF_SZ);
    int n, m;
    scanf("%d", &n);
    int i;
    for (i = 0; i < n; ++i) {
        scanf("%d", &x[i]);
    }
    for (i = 0; i < n; ++i) {
        scanf("%d", &y[i]);
    }
    qsort(x, n, sizeof(int), cmp);
    qsort(y, n, sizeof(int), cmp);
    scanf("%d", &m);
    int Px, Py;
    while (m-- > 0) {
        scanf("%d%d", &Px, &Py);
        printf("%d\n", search(Px, Py, n) + 1);
    }
    return 0;
}

int cmp(const void* p1, const void* p2)
{
    int i1 = *(const int*)p1;
    int i2 = *(const int*)p2;
    return (i1 > i2) - (i1 < i2);
}

int search(int Px, int Py, int n)
{
    int lo = 0, hi = n;
    while (lo < hi) {
        int mi = lo + ((hi - lo) >> 1);
        if (toleft(Px, Py, x[mi], y[mi])) {
            hi = mi;
        } else {
            lo = mi + 1;
        }
    }
    return lo - 1;
}

int toleft(int Px, int Py, int x, int y)
{
    return (cross(-x, y, Px - x, Py) > 0);
}

int64_t cross(int x1, int y1, int x2, int y2)
{
    return (int64_t)x1 * (int64_t)y2 - (int64_t)y1 * (int64_t)x2;
}
