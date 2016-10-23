#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define BUF_SZ (1 << 20)
#define ARRAY_SZ (1 << 23)
#define LOG2_LIST_SZ 7
#define MAXN 100000

char ibuf[BUF_SZ], obuf[BUF_SZ];

typedef struct Node {
    int idx;
    int val;
    struct Node* succ;
} Node;

typedef struct Pair {
    int idx;
    int val;
} Pair;

Node * pos[ARRAY_SZ], * neg[ARRAY_SZ];  // arrays of linked lists, with each list having at most 1 << LOG2_LIST_SZ elements
Pair cumpos[MAXN], cumneg[MAXN];    // cumulative +/- count, with descending idx (height) and ascending val (cumulative count)

void inc(Node* a[], int idx);    // increment the array of lists at the given index
int cum(Node* a[], Pair* cum);    // accumulate backward the values of the list nodes in the array of lists, store the partial sums in cum, and return the length of cum

Node* newnode(int idx, Node* succ);

int searchgeq(int e, Pair* a, int lo, int hi); // find the index of the first element in a[lo, hi) whose value >= e, hi on failure
int searchgt(int e, Pair* a, int lo, int hi); // find the index of the first element in a[lo, hi) whose value > e, hi on failure

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);

    int N, H, M, h;
    double Phit, Pfalse;
    char sign[2];

    // read in +/- distribution
    scanf("%d%d", &N, &H);
    cumpos[0] = cumneg[0] = (Pair) { .idx = H, .val = 0 };
    while (N-- > 0) {
        scanf("%d%s", &h, sign);
        switch (sign[0]) {
        case '+':
            inc(pos, h);
            break;
        case '-':
            inc(neg, h);
            break;
        default:
            break;
        }
    }

    // calculate cumulative values
    int lenpos = cum(pos, cumpos + 1) + 1, lenneg = cum(neg, cumneg + 1) + 1;

    // make judgements
    int minhit, maxfalse, ipos, ineg, hL, hH;
    scanf("%d", &M);
    while (M-- > 0) {
        scanf("%lf%lf", &Phit, &Pfalse);
        minhit = (int)ceil(Phit * cumpos[lenpos - 1].val);
        maxfalse = (int)floor(Pfalse * cumneg[lenneg - 1].val);
        ipos = searchgeq(minhit, cumpos, 0, lenpos);
        ineg = searchgt(maxfalse, cumneg, 0, lenneg);

        hL = ineg < lenneg ? cumneg[ineg].idx + 1 : 0;
        hH = ipos < lenpos ? cumpos[ipos].idx : -1;
        if (hL <= hH) {
            printf("%d %d\n", hL, hH);
        } else {
            printf("-1\n");
        }

    }

    return 0;
}

int searchgt(int e, Pair* a, int lo, int hi)
{
    while (lo < hi) {
        int mi = lo + ((hi - lo) >> 1);
        if (e < a[mi].val) {
            hi = mi;
        } else {
            lo = mi + 1;
        }
    }
    return lo;
}

int searchgeq(int e, Pair* a, int lo, int hi)
{
    return searchgt(e - 1, a, lo, hi);
}

void inc(Node* a[], int idx)
{
    int a_idx = idx >> LOG2_LIST_SZ;
    if (a[a_idx] == NULL) {
        a[a_idx] = newnode(idx, NULL);
    } else if (idx > a[a_idx]->idx){
        // descending by index in each list
        // insert as the first node
        a[a_idx] = newnode(idx, a[a_idx]);
    } else {
        Node * pb = a[a_idx], * pf = a[a_idx]->succ;
        while (pf != NULL && pf->idx > idx) {
            pb = pb->succ;
            pf = pf->succ;
        }
        if (pf != NULL && pf->idx == idx) {
            ++pf->val;
        } else {
            pb->succ = newnode(idx, pf);
        }
    }
}

Node* newnode(int idx, Node* succ)
{
    Node* newnode = (Node*)malloc(sizeof(Node));
    newnode->idx = idx;
    newnode->val = 1;
    newnode->succ = succ;
    return newnode;
}

int cum(Node* a[], Pair* cum)
{
    int i;
    int len = 0, sum = 0;
    for (i = ARRAY_SZ - 1; i >= 0; --i) {
        Node* node = a[i];
        while (node != NULL) {
            cum[len++] = (Pair) { .idx = node->idx, .val = (sum += node->val) };
            node = node->succ;
        }
    }
    return len;
}
