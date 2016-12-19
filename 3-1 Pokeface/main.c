#include <stdio.h>
#include <stdint.h>
#define BUF_SZ (1 << 20)

char ibuf[BUF_SZ], obuf[BUF_SZ];

/* ------------------------------------------
 * Segment tree interface
 * -----------------------------------------*/
void init(int range);
void reverse(int i, int j);
int query(int i, int j);

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);
    int n, p, q;
    scanf("%d%d%d", &n, &p, &q);
    init(n);
    int k;
    for (k = 0; k < p + q; ++k) {
        int op, i, j;
        scanf("%d%d%d", &op, &i, &j);
        switch (op) {
        case 0:
            reverse(i, j);
            break;
        case 1:
            printf("%d\n", query(i, j));
            break;
        default:
            break;
        }
    }
}


/* ------------------------------------------
 * Segment tree implementation
 * -----------------------------------------*/

#define MAXNODE (1 << 21)

typedef int ptr;

/* Lazy tag, inspired by https://www.cnblogs.com/Booble/archive/2010/10/11/1847793.html */

typedef struct {
    int l, r;
    uint8_t reverse;  // parity of reverse operations not propagated to children: 0 for even, 1 for odd
    int n_face; // number of faces in this range
    ptr lc, rc;
} Node;

Node pool[MAXNODE];
ptr _root;
int n_alloc = 0;
ptr alloc(int l, int r, ptr lc, ptr rc);    // allocate new node for [l, r]

inline void _reverse_val(Node* node);   // flip "reverse" and "n_face" for the given node
inline void _propagate(Node* node);     // propagate the delayed reverse operations at the given node to its children

ptr __init(int l, int r);   // initialize sub-tree for [l, r]

void init(int range)
{
    _root = __init(1, range);
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

void _reverse(ptr root, int i, int j);

void reverse(int i, int j)
{
    _reverse(_root, i, j);
}

void _reverse(ptr root, int i, int j)
{
    Node* p_root = pool + root;
    if (i > p_root->r || j < p_root->l) {
        return;
    }
    if (i <= p_root->l && j >= p_root->r) {
        _reverse_val(p_root);
    } else {
        _propagate(p_root);
        _reverse(p_root->lc, i, j);
        _reverse(p_root->rc, i, j);
        p_root->n_face = (pool + p_root->lc)->n_face + (pool + p_root->rc)->n_face;
    }
}

int _query(ptr root, int i, int j);

int query(int i, int j)
{
    return _query(_root, i, j);
}

int _query(ptr root, int i, int j)
{
    Node* p_root = pool + root;
    if (i > p_root->r || j < p_root->l) {
        return 0;
    }
    if (i <= p_root->l && j >= p_root->r) {
        return p_root->n_face;
    }
    _propagate(p_root);
    return _query(p_root->lc, i, j) + _query(p_root->rc, i, j);
}

ptr alloc(int l, int r, ptr lc, ptr rc)
{
    Node* curr = pool + n_alloc;
    curr->l = l;
    curr->r = r;
    curr->lc = lc;
    curr->rc = rc;
    curr->reverse = 0;
    curr->n_face = r + 1 - l;
    return n_alloc++;
}

void _reverse_val(Node* node)
{
    node->reverse = !node->reverse;
    node->n_face = node->r + 1 - node->l - node->n_face;
}

void _propagate(Node* node)
{
    if (node->reverse) {
        _reverse_val(pool + node->lc);
        _reverse_val(pool + node->rc);
        node->reverse = 0;
    }
}
