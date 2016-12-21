#include <stdio.h>
#define BUF_SZ (1 << 20)
#define MIN 0
#define MAX 1

char ibuf[BUF_SZ], obuf[BUF_SZ];

/*************************
 * DEPQ interface
 *************************/
void insert(int);
inline int popmax();    // return 0 if empty
inline int popmin();    // return 0 if empty

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);

    int n;
    scanf("%d", &n);

    char op[2];
    int pts;

    while (n-- > 0) {
        scanf("%s", op);
        switch (op[0]) {
            case 'I':
                scanf("%d", &pts);
                insert(pts);
                break;
            case 'H':
                printf("%d\n", popmax());
                break;
            case 'L':
                printf("%d\n", popmin());
                break;
            default:
                break;
        }
    }

    return 0;
}


/*************************
 * DEPQ implementation
 *************************/

#define MAXNODE 500000

typedef struct {
    int val;
    int r_min, r_max;   // ranks in minheap and maxheap
} Node;

Node pool[MAXNODE];
int minheap[MAXNODE], maxheap[MAXNODE];     // record index in pool
int pool_sz = 0, heap_sz = 0;

void percolateup(int whichheap, int rank);
void percolatedown(int whichheap, int sz);
void movetotop(int whichheap, int rank);
inline void del(int whichheap, int sz, int rank);

inline int getval(int idx);

int getval(int idx)
{
    return pool[idx].val;
}

int alloc(int val)
{
    Node* newnode = pool + pool_sz;
    newnode->val = val;
    return pool_sz++;
}

void insert(int val)
{
    int newaddr = alloc(val);
    minheap[heap_sz] = newaddr;
    maxheap[heap_sz] = newaddr;
    Node* newnode = pool + newaddr;
    newnode->r_max = newnode->r_min = heap_sz;
    percolateup(MIN, heap_sz);
    percolateup(MAX, heap_sz);
    ++heap_sz;
}

int pop(int which)
{
    if (!heap_sz) {
        return 0;
    }
    Node* node = pool + ((which == MAX) ? maxheap[0] : minheap[0]);
    del(MIN, heap_sz, node->r_min);
    del(MAX, heap_sz, node->r_max);
    --heap_sz;
    return node->val;
}

int popmax()
{
    return pop(MAX);
}

int popmin()
{
    return pop(MIN);
}

void swap(int whichheap, int r1, int r2)
{
    int* heap;
    (whichheap == MAX) ? (heap = maxheap) : (heap = minheap);
    int t;
    t = heap[r1];
    heap[r1] = heap[r2];
    heap[r2] = t;
    if (whichheap == MAX) {
        (pool + heap[r1])->r_max = r1;
        (pool + heap[r2])->r_max = r2;
    } else {
        (pool + heap[r1])->r_min = r1;
        (pool + heap[r2])->r_min = r2;
    }
}

void del(int whichheap, int sz, int rank)
{
    movetotop(whichheap, rank);
    swap(whichheap, sz - 1, 0);
    percolatedown(whichheap, sz - 1);
}

void percolateup(int whichheap, int rank)
{
    int* heap;
    (whichheap == MAX) ? (heap = maxheap) : (heap = minheap);
    while (rank) {
        int parent = (rank - 1) >> 1;
        if ((whichheap == MAX && getval(heap[parent]) >= getval(heap[rank]))
            || (whichheap == MIN && getval(heap[parent]) <= getval(heap[rank]))) {
            break;
        } else {
            swap(whichheap, rank, parent);
            rank = parent;
        }
    }
}

void movetotop(int whichheap, int rank)
{
    while (rank) {
        int parent = (rank - 1) >> 1;
        swap(whichheap, rank, parent);
        rank = parent;
    }
}

inline int argmax(int* heap, int r1, int r2);
inline int argmin(int* heap, int r1, int r2);

int argmax(int* heap, int r1, int r2)
{
    return (getval(heap[r1]) >= getval(heap[r2])) ? r1 : r2;
}

int argmin(int* heap, int r1, int r2)
{
    return (getval(heap[r1]) <= getval(heap[r2])) ? r1 : r2;
}

int properparent(int whichheap, int sz, int rank)
{
    int* heap;
    (whichheap == MAX) ? (heap = maxheap) : (heap = minheap);
    int rank_lc = (rank << 1) + 1, rank_rc = (rank + 1) << 1;
    if (rank_lc >= sz) {
        return rank;
    }
    if (rank_rc >= sz) {
        return ((whichheap == MAX) ? argmax(heap, rank, rank_lc) : argmin(heap, rank, rank_lc));
    }
    int properchild = ((whichheap == MAX) ? argmax(heap, rank_lc, rank_rc) : argmin(heap, rank_lc, rank_rc));
    return ((whichheap == MAX) ? argmax(heap, rank, properchild) : argmin(heap, rank, properchild));
}

void percolatedown(int whichheap, int sz)
{
    int rank = 0;
    int newparent;
    while (rank != (newparent = properparent(whichheap, sz, rank))) {
        swap(whichheap, rank, newparent);
        rank = newparent;
    }
}