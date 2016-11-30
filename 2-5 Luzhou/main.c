#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#define BUF_SZ (1 << 20)
#define MAXRES 500000
#define MAXTASK 350000
#define max(a, b) (((a) > (b)) ? (a) : (b))

char ibuf[BUF_SZ];

typedef struct {
    int cost;
    int value;
} Task;

// Comparator for tasks: use VALUE as the first key, COST as the second key; sort in DESCENDING order
inline int cmp(const void* p1, const void* p2);

Task resources[MAXRES];
Task tasks[MAXTASK];

/***************************
 * AVL Interface
 ***************************/
typedef int ptr;
ptr insert(ptr root, int k);
ptr find(ptr root, int k);   // find the node with the smallest key >= k
ptr del(ptr root, int k);
inline int getkey(ptr);


int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    int n, m;
    scanf("%d%d", &n, &m);
    int i;
    for (i = 0; i < n; ++i) {
        scanf("%d%d", &tasks[i].cost, &tasks[i].value);
    }
    for (i = 0; i < m; ++i) {
        scanf("%d%d", &resources[i].cost, &resources[i].value);
    }
    qsort(tasks, n, sizeof(Task), cmp);
    qsort(resources, m, sizeof(Task), cmp);
    
    uint64_t sum = 0;
    ptr root = -1;  // empty tree
    int j = 0;
    for (i = 0; i < n; ++i) {
        while (resources[j].value >= tasks[i].value) {
            root = insert(root, resources[j++].cost);
        }   // costs of resources whose values can cover the value of the current task are kept in the tree
        int k = getkey(find(root, tasks[i].cost));  // pick the cheapest one to afford the current task
        sum += (uint64_t)k;
        root = del(root, k);
    }
    printf("%lld\n", sum);
    return 0;
}

int cmp(const void* p1, const void* p2)
{
    Task * t1 = (Task *)p1, * t2 = (Task *)p2;
    if (t1->value < t2->value) {
        return 1;
    }
    if (t1->value == t2->value) {
        if (t1->cost < t2->cost) {
            return 1;
        }
        if (t1->cost == t2->cost) {
            return 0;
        }
        return -1;
    }
    return -1;
}

/***************************
 * AVL Implementation
 ***************************/

typedef struct {
    int key;
    ptr lc, rc;
    int8_t h;
} Node;

Node pool[MAXRES];
int n_alloc = 0;
inline ptr alloc(int k);

ptr balance(ptr root);
ptr findmin(ptr root);
ptr delmin(ptr root);
inline int8_t getheight(ptr);
inline void updateheight(ptr);
inline int8_t bf(ptr); // balance factor
ptr rotleft(ptr), rotright(ptr);

ptr insert(ptr root, int key)
{
    if (root == -1) {
        return alloc(key);
    }
    if (key < pool[root].key) {
        pool[root].lc = insert(pool[root].lc, key);
    } else {
        pool[root].rc = insert(pool[root].rc, key);
    }
    return balance(root);
}

ptr find(ptr root, int key)
{
    if (root == -1) {
        return -1;
    }
    if (key < pool[root].key) {
        ptr l = find(pool[root].lc, key);
        return (l == -1) ? root : l;
    }
    if (key == pool[root].key) {
        return root;
    }
    return find(pool[root].rc, key);
}

ptr del(ptr root, int key)
{
    if (root == -1) {
        return -1;
    }
    if (key == pool[root].key) {
        ptr l = pool[root].lc, r = pool[root].rc;
        if (r == -1) {
            return l;
        }
        ptr min = findmin(r);
        pool[min].rc = delmin(r);
        pool[min].lc = l;
        return balance(min);
    }
    if (key < pool[root].key) {
        pool[root].lc = del(pool[root].lc, key);
    } else {
        pool[root].rc = del(pool[root].rc, key);
    }
    return balance(root);
}

int getkey(ptr p)
{
    return pool[p].key;
}

ptr alloc(int key)
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
        if (bf(pool[root].lc) > 0) {
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
    updateheight(q);
    updateheight(p);
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
