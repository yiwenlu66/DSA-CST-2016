#include <stdio.h>
#include <stdint.h>
#define BUF_SZ (1 << 20)
#define MAXN 500000
#define WL 32   // word length, 32-bit integer with highest bit fixed at 0 in this problem

#define max(a, b) (((a) > (b)) ? (a) : (b))

const uint64_t MOD = ((uint64_t)1 << 31);

char ibuf[BUF_SZ], obuf[BUF_SZ];
uint32_t A[MAXN];


/* ------------------------------------------------------
 * Trie interface
 * ------------------------------------------------------*/
void insert(uint32_t x);        // insert x into the trie
uint32_t findMax(uint32_t x);   // find max({x^e: e is an element in the trie}); return 0 if trie is empty
void removeOldest();            // remove oldest element inserted into the trie


int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);

    int N, K;
    scanf("%d%d", &N, &K);
    int i;
    for (i = 0; i < N; ++i) {
        scanf("%u", &A[i]);
    }

    uint64_t E = 0;

    // A[i] ^ A[i+1] ^ ... ^ A[N-1], inspired by http://qpswwww.logdown.com/posts/332998-BZOJ-3261
    uint32_t postfix_xor = 0;

    for (i = N - 1; i >= 0; --i) {
        postfix_xor ^= A[i];
        uint32_t Ei = findMax(postfix_xor);
        if (i >= N - K) {
            Ei = max(Ei, postfix_xor);
        }
        E = (E + Ei) % MOD;
        insert(postfix_xor);
        if (i < N - K) {
            removeOldest();
        }
    }

    printf("%lld\n", E);
    return 0;
}


/* ------------------------------------------------------
 * Trie implementation
 * ------------------------------------------------------*/

#define MAX_INTERNAL 6500000

typedef struct {
    /*
     * Indices of child nodes.
     * children[0] for left child (stands for bit 0); children[1] for right child (stands for bit 1).
     * Values:
     *  - [0, MAX_INTERNAL) for index in internal node pool.
     *  - [MAX_INTERNAL, MAX_INTERNAL + MAXN) for index in leaf node pool.
     *  - -1 for non-existence.
     */
    int children[2];
    int parent; // index of parent node
} InternalNode;


typedef struct {
    /*
     * Multiplicity for repeated numbers.
     * Increment on insertion;
     * decrement on removal;
     * remove node (and possibly parents and grandparents) when decremented to 0.
     */
    int mul;
    int parent; // index of parent node
} LeafNode;

InternalNode pool_internal[MAX_INTERNAL];
LeafNode pool_leaf[MAXN];
int sz_internal = 0, sz_leaf = 0;
int alloc_internal(int parent), alloc_leaf(int parent);

int rmqueue[MAXN];  // record the indices of LEAF nodes in the order they're inserted; dequeue on removal
int rmqueue_front = 0, rmqueue_rear = 0;
void enqueue(int i);
int dequeue();

/* helper functions */
void getbits(uint8_t* dst, uint32_t src);   // store bits in big endian
int getnchildren(int node_idx);

int root = -1;  // not initialized


void insert(uint32_t x)
{
    if (root == -1) {
        root = alloc_internal(-1);
    }

    uint8_t bits[WL];
    getbits(bits, x);

    int current = root;
    int i;
    for (i = 1; i < WL - 1; ++i) {
        // skip MSB (always 0)
        int* child_ptr = &pool_internal[current].children[bits[i]];
        if (*child_ptr == -1) {
            *child_ptr = alloc_internal(current);
        }
        current = *child_ptr;
    }

    int* child_ptr = &pool_internal[current].children[bits[i]];
    if (*child_ptr == -1) {
        *child_ptr = alloc_leaf(current) + MAX_INTERNAL;
    } else {
        ++pool_leaf[*child_ptr - MAX_INTERNAL].mul;
    }
    enqueue(*child_ptr - MAX_INTERNAL);
}

uint32_t findMax(uint32_t x)
{
    if (root == -1) {
        return 0;
    }

    uint8_t bits[WL];
    getbits(bits, x);

    uint32_t result = 0;
    int current = root;
    int i;
    for (i = 1; i < WL; ++i) {
        // try to get the inverse bit first
        int* child_ptr = &pool_internal[current].children[bits[i] ^ 1];
        if (*child_ptr != -1) {
            result = (result << 1) + 1;
        } else {
            // inverse bit does not exist, use same bit
            child_ptr = &pool_internal[current].children[bits[i]];
            result <<= 1;
        }
        current = *child_ptr;
    }
    
    return result;
}

void removeOldest()
{
    int leaf_idx = dequeue();

    // try to decrement multiplicity first
    if (pool_leaf[leaf_idx].mul > 1) {
        --pool_leaf[leaf_idx].mul;
        return;
    }

    // remove single node from the tree
    int parent = pool_leaf[leaf_idx].parent;
    if (getnchildren(parent) == 2) {
        if (pool_internal[parent].children[0] == leaf_idx + MAX_INTERNAL) {
            pool_internal[parent].children[0] = -1;
        } else{
            pool_internal[parent].children[1] = -1;
        }
        return;
    }

    int current;
    do {
        current = parent;
        parent = pool_internal[current].parent;
    } while (current != root && getnchildren(parent) == 1);
    if (current == root) {
        root = -1;
        return;
    }
    if (pool_internal[parent].children[0] == current) {
        pool_internal[parent].children[0] = -1;
    } else{
        pool_internal[parent].children[1] = -1;
    }
}

int alloc_internal(int parent)
{
    InternalNode* p = &pool_internal[sz_internal];
    p->parent = parent;
    p->children[0] = p->children[1] = -1;
    return sz_internal++;
}

int alloc_leaf(int parent)
{
    LeafNode* p = &pool_leaf[sz_leaf];
    p->parent = parent;
    p->mul = 1;
    return sz_leaf++;
}

void enqueue(int i)
{
    rmqueue[rmqueue_front++] = i;
}

int dequeue()
{
    return rmqueue[rmqueue_rear++];
}

void getbits(uint8_t* dst, uint32_t src)
{
    int i;
    for (i = WL - 1; i >= 0; --i, src >>= 1) {
        dst[i] = src & 1;
    }
}

int getnchildren(int node_idx)
{
    InternalNode* p = &pool_internal[node_idx];
    if (p->children[0] == -1 && p->children[1] == -1) {
        return 0;
    }
    if (p->children[0] == -1 || p->children[1] == -1) {
        return 1;
    }
    return 2;
}
