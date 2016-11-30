#include <stdio.h>
#include <stdint.h>
#define BUF_SZ (1 << 20)
#define MAXNODE 500000
#define MAXLOOKUP 350000
#define max(a, b) (((a) > (b)) ? (a) : (b))

char ibuf[BUF_SZ];

typedef struct {
    int p;
    int v;
} Pair;

inline int cmp(Pair p1, Pair p2);

Pair lookup[MAXLOOKUP];

/***************************
 * AVL Interface
 ***************************/
typedef int ptr;
ptr insert(ptr root, Pair key);
ptr find(ptr root, Pair key);   // find the node with the smallest key >= the given key
ptr del(ptr root, Pair key);
inline Pair getkey(ptr);


int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    int n, m;
    scanf("%d%d", &n, &m);
    int i;
    for (i = 0; i < n; ++i) {
        scanf("%d%d", &lookup[i].p, &lookup[i].v);
    }
    int root = -1;  // empty tree
    for (i = 0; i < m; ++i) {
        int h, d;
        scanf("%d%d", &h, &d);
        root = insert(root, (Pair) { .p = h, .v = d });
    }
    int sum = 0;
    for (i = 0; i < n; ++i) {
        Pair key = getkey(find(root, lookup[i]));
        sum += key.p;
        root = del(root, key);
    }
    printf("%d\n", sum);
    return 0;
}

int cmp(Pair p1, Pair p2)
{
    if (p1.p < p2.p) {
        return -1;
    }
    if (p1.p == p2.p) {
        if (p1.v < p2.v) {
            return -1;
        }
        if (p1.v == p2.v) {
            return 0;
        }
        return 1;
    }
    return 1;
}

/***************************
 * AVL Implementation
 ***************************/

typedef struct {
    Pair key;
    ptr lc, rc;
    int8_t h;
} Node;

Node pool[MAXNODE];
int n_alloc = 0;
inline ptr alloc(Pair key);

ptr balance(ptr root);
ptr findmin(ptr root);
ptr delmin(ptr root);
inline int8_t getheight(ptr);
inline void updateheight(ptr);
inline int8_t bf(ptr); // balance factor
ptr rotleft(ptr), rotright(ptr);

ptr insert(ptr root, Pair key)
{
    if (root == -1) {
        return alloc(key);
    }
    if (cmp(key, pool[root].key) == -1) {
        pool[root].lc = insert(pool[root].lc, key);
    } else {
        pool[root].rc = insert(pool[root].rc, key);
    }
    return balance(root);
}

ptr find(ptr root, Pair key)
{
    if (root == -1) {
        return -1;
    }
    switch (cmp(key, pool[root].key)) {
    case -1: {
        ptr l = find(pool[root].lc, key);
        return (l == -1) ? root : l;
    }
    case 0:
        return root;
    case 1: default:
        return find(pool[root].rc, key);
    }
}

ptr del(ptr root, Pair key)
{
    if (root == -1) {
        return -1;
    }
    switch (cmp(key, pool[root].key)) {
    case -1:
        pool[root].lc = del(pool[root].lc, key);
        break;
    case 1:
        pool[root].rc = del(pool[root].rc, key);
        break;
    case 0: {
        ptr l = pool[root].lc, r = pool[root].rc;
        if (r == -1) {
            return l;
        }
        ptr min = findmin(r);
        pool[min].rc = delmin(r);
        pool[min].lc = l;
        return balance(min);
    }
    }
    return balance(root);
}

Pair getkey(ptr p)
{
    return pool[p].key;
}

ptr alloc(Pair key)
{
    pool[n_alloc].key = key;
    pool[n_alloc].lc = pool[n_alloc].rc = -1;
    pool[n_alloc].h = 1;
    return n_alloc++;
}

ptr balance(ptr root)
{
    updateheight(root);
    switch (bf(root)) {
    case 2:
        if (bf(pool[root].rc) < 0) {
            pool[root].rc = rotright(pool[root].rc);
        }
        return rotleft(root);
    case -2:
        if (bf(pool[root].lc) < 0) {
            pool[root].lc = rotleft(pool[root].lc);
        }
        return rotright(root);
    default:
        return root;
    }
}

ptr findmin(ptr root)
{
    return (pool[root].lc == -1) ? root : findmin(pool[root].lc);
}

ptr delmin(ptr root)
{
    if (pool[root].lc == -1) {
        return pool[root].rc;
    }
    pool[root].lc = delmin(pool[root].lc);
    return balance(root);
}

int8_t getheight(ptr root)
{
    return (root == -1) ? 0 : pool[root].h;
}

void updateheight(ptr root)
{
    pool[root].h = max(getheight(pool[root].lc), getheight(pool[root].rc)) + 1;
}

int8_t bf(ptr root)
{
    return getheight(pool[root].rc) - getheight(pool[root].lc);
}

ptr rotleft(ptr q)
{
    ptr p = pool[q].rc;
    pool[q].rc = pool[p].lc;
    pool[p].lc = q;
    updateheight(p);
    updateheight(q);
    return p;
}

ptr rotright(ptr p)
{
    ptr q = pool[p].lc;
    pool[p].lc = pool[q].rc;
    pool[q].rc = p;
    updateheight(p);
    updateheight(q);
    return q;
}
