#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define IBUF_SZ 1 << 23
#define OBUF_SZ 1 << 21
#define MAXN 1000001
#define TH 3    // threshold for elimination

typedef struct Node {
    char val;
    struct Node* pred;
    struct Node* succ;
} Node;

char ibuf[IBUF_SZ], obuf[OBUF_SZ];
char init_list[MAXN];

Node * head, * trailer;
size_t len = 0;

// list operations
void init();
void append(char val);
void insertbefore(Node* newnode, Node* succ);
void insertafter(Node* newnode, Node* pred);
void del(Node* n);
void output();

void insert(char val, size_t pos);  // insert and try to eliminate

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, IBUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, OBUF_SZ);

    // initialize list
    init();
    scanf("%s", init_list);
    size_t i;
    for (i = 0; i < strlen(init_list); ++i) {
        append(init_list[i]);
    }

    // emulate operations
    int m;
    scanf("%d", &m);
    size_t pos;
    char vals[2];
    while (m-- > 0) {
        scanf("%lu%s", &pos, vals);
        insert(vals[0], pos);
    }

    output();

    return 0;
}

// functions related to elimination
Node* getelimnode(Node* n);   // return the starting node of the eliminatable streak that n belongs to, NULL if not existing
Node* elim(Node* start);   // given the starting node of a eliminatable streak, eliminate and return the node just BEFORE it

void init()
{
    head = (Node*)malloc(sizeof(Node));
    trailer = (Node*)malloc(sizeof(Node));
    head->val = trailer->val = '\0';
    head->pred = NULL;
    head->succ = trailer;
    trailer->pred = head;
    trailer->succ = NULL;
}

void append(char val)
{
    Node* newnode = (Node*)malloc(sizeof(Node));
    newnode->val = val;
    insertbefore(newnode, trailer);
}

void insert(char val, size_t pos)
{
    // insert node
    Node* newnode = (Node*)malloc(sizeof(Node));
    newnode->val = val;
    Node* pred = head;
    while (pos-- > 0) {
        pred = pred->succ;
    }
    insertafter(newnode, pred);

    // try to eliminate
    Node* elimnode = getelimnode(newnode);
    while (elimnode != NULL) {
        elimnode = getelimnode(elim(elimnode));
    }
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

void insertafter(Node* newnode, Node* pred)
{
    newnode->pred = pred;
    newnode->succ = pred->succ;
    pred->succ->pred = newnode;
    pred->succ = newnode;
    ++len;
}

void insertbefore(Node* newnode, Node* succ)
{
    newnode->pred = succ->pred;
    newnode->succ = succ;
    succ->pred->succ = newnode;
    succ->pred = newnode;
    ++len;
}

void del(Node* n)
{
    n->pred->succ = n->succ;
    n->succ->pred = n->pred;
    free(n);
    --len;
}

Node* getelimnode(Node* n)
{
    // edge case
    if (n == head || n == trailer) {
        return NULL;
    }

    int streak = 0;

    // search backward
    Node* start = n;
    while (start->val == n->val) {
        ++streak;
        start = start->pred;
    }
    start = start->succ;

    // search forward
    Node* offend = n->succ;
    while (offend->val == n->val) {
        ++streak;
        offend = offend->succ;
    }

    if (streak >= TH) {
        return start;
    }
    return NULL;
}

Node* elim(Node* start)
{
    Node* ret = start->pred;
    char val = start->val;
    while (ret->succ->val == val) {
        del(ret->succ);
    }
    return ret;
}
