#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#define BUF_SZ (1 << 20)
#define MAXTARGET 100000

char ibuf[BUF_SZ], obuf[BUF_SZ];


typedef struct {
    int l;
    int r;
    int w;
} Target;

Target targets[MAXTARGET];
inline int cmp(const void* p1, const void* p2);    // sort targets by weight


/***************************************
 * Persistent segment tree interface
 ***************************************/

void init(int range);   // range <= 100000; legal positions will be within [1, range]
void insert(int l, int r, int w);   // insert segment [l, r] with weight w

/* get the sum of weights of the first k segments inserted at position x 
 * return the sum of weights of all segments at x if there are less than k */
uint64_t query(int x, int k);


int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);
    int m, n;
    scanf("%d%d", &m, &n);
    int i;

    // read in targets, sort by weight, and construct segment tree
    for (i = 0; i < m; ++i) {
        scanf("%d%d%d", &targets[i].l, &targets[i].r, &targets[i].w);
    }
    qsort(targets, m, sizeof(Target), cmp);
    init(n);
    for (i = 0; i < m; ++i) {
        insert(targets[i].l, targets[i].r, targets[i].w);
    }

    // perform queries
    int x, k;
    uint64_t a, b, c;
    uint64_t pre = 1;
    for (i = 0; i < n; ++i) {
        scanf("%d%lld%lld%lld", &x, &a, &b, &c);
        k = (int)(((a % c) * (pre % c) + (b % c)) % c + 1);
        pre = query(x, k);
        printf("%lld\n", pre);
    }

    return 0;
}


int cmp(const void* p1, const void* p2)
{
    Target * t1 = (Target *) p1, * t2 = (Target *) p2;
    return (t1->w > t2->w) - (t1->w < t2->w);
}


/********************************************
 * Persistent segment tree implementation
 * Inspired by https://en.wikipedia.org/wiki/Segment_tree
 * and https://blog.anudeep2011.com/persistent-segment-trees-explained-with-spoj-problems/
 ********************************************/

#define MAXRANGE 100000
#define MAXVERSION 100000
#define MAXNODE 4096000

typedef int ptr;

typedef struct {
    int l;
    int r;
    int n;  // # segments covering this but not parent
    uint64_t sw;    // sum(weight) of segments covering this but not parent
    ptr lc; // left child
    ptr rc; // right child
} Node;

int _range;

Node pool[MAXNODE];
ptr n_alloc = 0;
ptr alloc(int l, int r, ptr lc, ptr rc);    // allocate new node for [l, r]

ptr roots[MAXVERSION];
int n_version = 0;

inline Node* getnode(ptr p);
inline Node* getlc(ptr p);
inline Node* getrc(ptr p);

inline void getdata(int pos, int ver, int* n, uint64_t* sw); // find n and sw at the given position and version

ptr __init(int l, int r);  // initialize sub-tree for [l, r];

void init(int range)
{
    _range = range;
    roots[0] = __init(1, range);
}

ptr __init(int l, int r)
{
    if (l == r) {
        // leaf node
        return alloc(l, r, -1, -1);
    }
    int m = l + ((r - l) >> 1);
    return alloc(l, r, __init(l, m), __init(m + 1, r));
}

void _insert(ptr root, int l, int r, int w);
void _insert_ver(ptr newroot, ptr oldroot, int l, int r, int w);    // insert as a new version

void insert(int l, int r, int w)
{
    if (!n_version) {
        // empty tree
        _insert(roots[0], l, r, w);
        ++n_version;
    } else {
        roots[n_version++] = alloc(1, _range, -1, -1);
        _insert_ver(roots[n_version - 1], roots[n_version - 2], l, r, w);
    }
}

void _insert(ptr root, int l, int r, int w)
{
    Node* _root = getnode(root);
    if (l <= _root->l && r >= _root->r) {
        // the segment covers the current root
        ++(_root->n);
        _root->sw += w;
        return;
    }
    if (l <= getlc(root)->r) {
        // intersects left child
        _insert(_root->lc, l, r, w);
    }
    if (r >= getrc(root)->l) {
        // intersects right child
        _insert(_root->rc, l, r, w);
    }
}

void _insert_ver(ptr newroot, ptr oldroot, int l, int r, int w)
{
    Node * _newroot = getnode(newroot), * _oldroot = getnode(oldroot);
    if (l <= _oldroot->l && r >= _oldroot->r) {
        // the segment covers the current root
        _newroot->lc = _oldroot->lc;
        _newroot->rc = _oldroot->rc;
        _newroot->n = _oldroot->n + 1;
        _newroot->sw = _oldroot->sw + w;
        return;
    }

    // root remains unchanged, recursively update children as needed
    _newroot->n = _oldroot->n;
    _newroot->sw = _oldroot->sw;
    Node * _oldlc = getlc(oldroot), * _oldrc = getrc(oldroot);

    if (l <= _oldlc->r) {
        // intersects left child
        _newroot->lc = alloc(_oldlc->l, _oldlc->r, -1, -1);
        _insert_ver(_newroot->lc, _oldroot->lc, l, r, w);
    } else {
        _newroot->lc = _oldroot->lc;
    }

    if (r >= _oldrc->l) {
        // intersects right child
        _newroot->rc = alloc(_oldrc->l, _oldrc->r, -1, -1);
        _insert_ver(_newroot->rc, _oldroot->rc, l, r, w);
    } else {
        _newroot->rc = _oldroot->rc;
    }
}

uint64_t query(int pos, int k)
{
    // # segments at x is non-decreasing; binary search for the root of the corresponding version
    int lo = 0, hi = n_version;
    int ver;
    int n;
    uint64_t sw = 0;
    while (lo < hi) {
        ver = lo + ((hi - lo) >> 1);
        getdata(pos, ver, &n, &sw);
        if (n == k) {
            break;
        } else if (n > k) {
            hi = ver;
        } else {
            lo = ver + 1;
        }
    }
    if (lo == hi) {
        // not found, use the latest version
        ver = n_version - 1;
        getdata(pos, ver, &n, &sw);
    }
    return sw;
}

void _getdata(int pos, ptr root, int* n, uint64_t* sw);

void getdata(int pos, int ver, int* n, uint64_t* sw)
{
    *sw = 0;
    *n = 0;
    _getdata(pos, roots[ver], n, sw);
}

void _getdata(int pos, ptr root, int* n, uint64_t* sw)
{
    Node * _root = getnode(root), * _lc = getlc(root), * _rc = getrc(root);
    *n += _root->n;
    *sw += _root->sw;
    if (_lc && pos <= _lc->r) {
        _getdata(pos, _root->lc, n, sw);
    } else if (_rc && pos >= _rc->l) {
        _getdata(pos, _root->rc, n, sw);
    }
}

ptr alloc(int l, int r, ptr lc, ptr rc)
{
    Node* curr = pool + n_alloc;
    curr->l = l;
    curr->r = r;
    curr->lc = lc;
    curr->rc = rc;
    curr->n = curr->sw = 0;
    return n_alloc++;
}

Node* getnode(ptr p)
{
    return (p == -1) ? NULL : (pool + p);
}

Node* getlc(ptr p)
{
    return (pool[p].lc == -1) ? NULL : (pool + pool[p].lc);
}

Node* getrc(ptr p)
{
    return (pool[p].rc == -1) ? NULL : (pool + pool[p].rc);
}
