#include "temperature.h"
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#define MAXN 50000
#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef struct {
    int x;
    int y;
    int temp;
} Station;

Station stations[MAXN];
int y[MAXN];

int deduplicate(int* a, int old_size);  // return the new size

inline int cmp_int(const void* p1, const void* p2);
inline int cmp_station(const void* p1, const void* p2);    // sort stations by x coordinate

inline void swap(int* a, int* b);

/*************************************
 * Persistent segment tree interface
 *************************************/
void init(int* y, int y_size);    // initialize a segment tree for the given end points (already sorted)
// create a new version for the x coordinate and insert a value at the y coordinate;
// x must be non-decreasing over calls
void insert(int x, int y, int val);
// query the number and sum of inserted values within [y1, y2] at the specific version
void query(int ver, int y1, int y2, int* num, int64_t* sum);
#define INCLUDE 0
#define EXCLUDE 1
// find the latest version number with inserted x coordinates <= x (when flag == INCLUDE) or < x (when flag == EXCLUDE)
int getversion(int x, int flag);


int main()
{
    // read data
    int n_station = GetNumOfStation();
    int i;
    for (i = 0; i < n_station; ++i) {
        GetStationInfo(i, &stations[i].x, &stations[i].y, &stations[i].temp);
        y[i] = stations[i].y;
    }

    // construct tree
    qsort(y, (size_t)n_station, sizeof(int), cmp_int);
    int y_size = deduplicate(y, n_station);
    init(y, y_size);
    qsort(stations, (size_t)n_station, sizeof(Station), cmp_station);
    for (i = 0; i < n_station; ++i) {
        insert(stations[i].x, stations[i].y, stations[i].temp);
    }

    // process queries
    int x1, y1, x2, y2;
    while (GetQuery(&x1, &y1, &x2, &y2)) {
        if (x1 > x2) {
            swap(&x1, &x2);
        }
        if (y1 > y2) {
            swap(&y1, &y2);
        }
        int num_i, num_t;
        int64_t sum_i, sum_t;
        // get the number and sum of stations for the initial and terminal state;
        // get the answer by calculating the difference
        query(getversion(x1, EXCLUDE), y1, y2, &num_i, &sum_i);
        query(getversion(x2, INCLUDE), y1, y2, &num_t, &sum_t);
        Response((num_t != num_i) ? (int)((sum_t - sum_i) / (num_t - num_i)) : 0);
    }

    return 0;
}

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int deduplicate(int* a, int old_size)
{
    int i = 0, j = 0;
    while (++j < old_size) {
        if (a[i] != a[j]) {
            a[++i] = a[j];
        }
    }
    return ++i;
}

int cmp_int(const void* p1, const void* p2)
{
    int i1 = *(const int*)p1;
    int i2 = *(const int*)p2;
    return (i1 > i2) - (i1 < i2);
}

int cmp_station(const void* p1, const void* p2)
{
    int x1 = ((const Station*)p1)->x;
    int x2 = ((const Station*)p2)->x;
    return (x1 > x2) - (x1 < x2);
}

/*************************************
 * Persistent segment tree implementation
 *************************************/

typedef int ptr;

typedef struct {
    int lo, hi;
    ptr lc, rc; // -1 for non-existent
    int num;    // number of values inserted within [lo, hi]
    int64_t sum;   // sum of values inserted within [lo, hi]
} Node;

#define MAXNODE (1 << 20)
#define MAXVERSION (50000 + 1)

Node pool[MAXNODE];
ptr top = 0;

ptr alloc(int lo, int hi, ptr lc, ptr rc);

ptr roots[MAXVERSION];
ptr x[MAXVERSION];  // x[i]: the maximum x inserted up till version i
// roots[0] and x[0] are reserved for the empty tree
int latest;

ptr __init(int *y, int y_size);

void init(int* y, int y_size)
{
    roots[0] = __init(y, y_size);
    x[0] = INT_MIN;
    latest = 0;
}

ptr __init(int *y, int y_size)
{
    if (y_size == 1) {
        return alloc(*y, *y, -1, -1);
    }
    return alloc(
            *y,
            *(y + y_size - 1),
            __init(y, y_size >> 1),
            __init(y + (y_size >> 1), y_size - (y_size >> 1))
    );
}

ptr _insert(ptr oldroot, int y, int val);   // return the new root

void insert(int _x, int y, int val)
{
    roots[latest + 1] = _insert(roots[latest], y, val);
    x[++latest] = _x;
}

ptr _insert(ptr oldrootptr, int y, int val)
{
    Node* old = pool + oldrootptr;
    ptr newrootptr = alloc(old->lo, old->hi, -1, -1);
    Node* new = pool + newrootptr;
    new->num = old->num + 1;
    new->sum = old->sum + val;
    if (old->lc != -1) {
        if (y <= (pool + old->lc)->hi) {
            new->lc = _insert(old->lc, y, val);
            new->rc = old->rc;
        } else {
            new->lc = old->lc;
            new->rc = _insert(old->rc, y, val);
        }
    }
    return newrootptr;
}

// query incrementally, i.e. ADD the values into num and sum
void _query(ptr root, int lo, int hi, int* num, int64_t* sum);

void query(int ver, int y1, int y2, int* num, int64_t* sum)
{
    *num = *sum = 0;
    _query(roots[ver], y1, y2, num, sum);
}

void _query(ptr rootptr, int lo, int hi, int* num, int64_t* sum)
{
    Node* root = pool + rootptr;

    // trivial case: [lo, hi] does not intersect the range of root
    if (hi < root->lo || lo > root->hi) {
        return;
    }

    // trivial case: [lo, hi] covers the range of root
    if (lo <= root->lo && hi >= root->hi) {
        *num += root->num;
        *sum += root->sum;
        return;
    }

    // query recursively
    int lh = (pool + root->lc)->hi, rl = (pool + root->rc)->lo;
    if (lo <= lh) {
        _query(root->lc, lo, hi, num, sum);
    }
    if (hi >= rl) {
        _query(root->rc, lo, hi, num, sum);
    }
}

int getversion(int _x, int flag)
{
    // binary search
    int lo = 0, hi = latest + 1;
    while (lo < hi) {
        int mi = lo + ((hi - lo) >> 1);
        if ((flag == INCLUDE) ? (_x < x[mi]) : (_x <= x[mi])) {
            hi = mi;
        } else {
            lo = mi + 1;
        }
    }
    return max(0, lo - 1);
}

ptr alloc(int lo, int hi, ptr lc, ptr rc)
{
    Node* new = pool + top;
    new->lo = lo;
    new->hi = hi;
    new->lc = lc;
    new->rc = rc;
    new->num = new->sum = 0;
    return top++;
}
