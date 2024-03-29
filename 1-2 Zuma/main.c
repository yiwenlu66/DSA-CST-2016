#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define max(a, b) ((a) > (b)) ? (a) : (b)
#define min(a, b) ((a) < (b)) ? (a) : (b)
#define IBUF_SZ 1 << 23
#define OBUF_SZ 1 << 21
#define MAXN 1000001
#define MAXIDX 1250
#define MINSTEPSZ 10

typedef struct Node {
    char val;
    struct Node* pred;
    struct Node* succ;
} Node;

char ibuf[IBUF_SZ], obuf[OBUF_SZ];
char init_str[MAXN];

Node * head, * trailer;

// list operations
void init();
void insertbefore(Node* newnode, Node* succ);
void output();

void newbead(char val, size_t pos);  // insert or eliminate; update index

// index for speeding up addressing, inspired by ex2-23
size_t len;
Node* idx[MAXIDX];   // pointers to node 0, stepsz, 2 * stepsz, ...
size_t stepsz, halfstepsz, nstep;    // stepsz ~ sqrt(m + n)

// insertion and elimination
Node* append(char val);  // create a new node and append it to the end; for initialization only
void insert(Node* succ, char val, size_t pos);   // create a new node and insert it before the given node; update index
void elim(Node* start, size_t startpos, size_t n);   // delete n nodes starting from the given node; update index

// others
size_t readline(char* s);

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, IBUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, OBUF_SZ);


    // estimate scale and initialize list

    init();
    len = readline(init_str);

    size_t m;
    scanf("%lu", &m);
    stepsz = max(MINSTEPSZ, (size_t)sqrt(len + m));
    halfstepsz = stepsz >> 1;

    size_t i;
    for (i = nstep = 0; i < len; ++i) {
        Node* newnode = append(init_str[i]);
        if (i % stepsz == 0) {
            idx[nstep++] = newnode;
        }
    }


    // emulate operations
    size_t pos;
    char vals[2];
    while (m-- > 0) {
        scanf("%lu%s", &pos, vals);
        newbead(vals[0], pos);
    }

    output();

    return 0;
}

void init()
{
    head = (Node*)malloc(sizeof(Node));
    trailer = (Node*)malloc(sizeof(Node));
    head->val = '^';
    trailer->val = '$';
    head->pred = NULL;
    head->succ = trailer;
    trailer->pred = head;
    trailer->succ = NULL;
}

Node* append(char val)
{
    Node* newnode = (Node*)malloc(sizeof(Node));
    newnode->val = val;
    insertbefore(newnode, trailer);
    return newnode;
}

void newbead(char val, size_t pos)
{
    // (lo, hi) can be eliminated if the range is long enough
    size_t streak = 1;
    Node * lo, * hi;
    if (pos % stepsz <= halfstepsz) {
        hi = pos / stepsz < nstep ? idx[pos / stepsz] : trailer;
        size_t i;
        for (i = 0; i < pos % stepsz; ++i) {
            hi = hi->succ;
        }
    } else {
        hi = pos / stepsz + 1 < nstep ? idx[pos / stepsz + 1] : trailer;
        size_t i;
        size_t offset = (hi == trailer) ? (len - pos) : (stepsz - pos % stepsz);
        for (i = 0; i < offset; ++i) {
            hi = hi->pred;
        }
    }
    lo = hi->pred;
    size_t lopos = pos - 1;
    while (lo->val == val) {
        ++streak;
        lo = lo->pred;
        --lopos;
    }
    Node* succ = hi; // back up the initial position of hi, i.e. the successor of the new node
    while (hi->val == val) {
        ++streak;
        hi = hi->succ;
    }
    if (streak < 3) {
        // cannot eliminate, insert the node
        insert(succ, val, pos);
    } else {
        // can eliminate, no need to insert the node
        size_t n = 2; // total number of nodes to be eliminated, excluding the "new" node
        int loop = 1;
        while (loop && lo->val == hi->val) {
            // can do furthur elimination

            int lc = (lo->pred->val == lo->val);   // can continue left
            int rc = (hi->succ->val == hi->val);   // can continue right

            switch ((lc << 1) + rc) {
            case 0:
                loop = 0;
                break;
            case 1:
                lo = lo->pred;
                --lopos;
                hi = hi->succ->succ;
                n += 3;
                break;
            case 2:
                lo = lo->pred->pred;
                lopos -= 2;
                hi = hi->succ;
                n += 3;
                break;
            case 3:
                lo = lo->pred->pred;
                lopos -= 2;
                hi = hi->succ->succ;
                n += 4;
                break;
            default:
                break;
            }

        }
        elim(lo->succ, lopos + 1, n);
    }
}

void insert(Node* succ, char val, size_t pos)
{
    // insert the node
    Node* newnode = (Node*)malloc(sizeof(Node));
    newnode->val = val;
    insertbefore(newnode, succ);

    // update index
    size_t i;
    for (i = pos ? (pos - 1) / stepsz + 1 : 0; i < nstep; ++i) {
        idx[i] = idx[i]->pred;
    }
    if (++len % stepsz == 1) {
        idx[nstep++] = trailer->pred;
    }
}

void elim(Node* start, size_t startpos, size_t n)
{
    // update the index
    size_t quo = n / stepsz, mod = n % stepsz;
    size_t i, j;
    len -= n;
    nstep = len ? (len - 1) / stepsz + 1 : 0;
    for (i = startpos ? (startpos - 1) / stepsz + 1 : 0; i < nstep; ++i) {
        idx[i] = idx[i + quo];
        for (j = 0; j < mod; ++j) {
            idx[i] = idx[i]->succ;
        }
    }

    // delete the nodes
    Node* before = start->pred;
    Node* after = start;
    for (i = 0; i < n; ++i) {
        after = after->succ;
        free(after->pred);
    }
    before->succ = after;
    after->pred = before;
}

void output()
{
    Node* curr = head->succ;
    while (curr->succ != NULL) {
        putchar(curr->val);
        curr = curr->succ;
    }
    putchar('\n');
}

void insertbefore(Node* newnode, Node* succ)
{
    newnode->pred = succ->pred;
    newnode->succ = succ;
    succ->pred->succ = newnode;
    succ->pred = newnode;
}

size_t readline(char* s)
{
    size_t i = 0;
    char c;
    while ((c = getchar()) != '\n') {
        s[i++] = c;
    }
    s[i] = '\0';
    return i;
}
