#include <stdio.h>
#include <ctype.h>
#define BUF_SZ 1 << 20
#define MAXNODE 8000002

#define FAIL printf("F\n");
#define SUCC printf("T\n");
#define GETCUR cur = (getnbchar() == 'L') ? &lcur : &rcur;

char ibuf[BUF_SZ], obuf[BUF_SZ];

typedef struct Node {
    char c;
    struct Node* pred;
    struct Node* succ;
} Node;

// pre-allocate all nodes to increase efficiency
Node pool[MAXNODE];
int top;

char getnbchar();   // read non-blank character from stdin

// linked list
Node * head, * trailer;
void init();    // read a string from stdin and initialize the list
void insertbefore(Node* n, char c);
void del(Node* n);
void reverse(Node* lo, Node* hi);
void show();


int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);

    init();
    Node * lcur = head->succ, * rcur = trailer; // point to the node that the cursor is to the LEFT of
    /*
     * cursor status
     * -1: lcur is to the left of rcur
     *  0: lcur and rcur are the same
     *  1: lcur is to the left of rcur
     */
    int cursts = -1;

    int n;
    scanf("%d", &n);
    while (n-- > 0) {
        Node** cur;

        switch (getnbchar()) {

        case '<':
            GETCUR;
            if ((*cur)->pred == head) {
                // cursor is already to the left of the leftmost character
                FAIL;
            } else {
                SUCC;
                *cur = (*cur)->pred;
                // update cursor status
                if (!cursts) {
                    cursts = (*cur == lcur) ? -1 : 1;
                } else if (lcur == rcur) {
                    cursts = 0;
                }
            }
            break;

        case '>':
            GETCUR;
            if (*cur == trailer) {
                // cursor is already to the right of the rightmost character
                FAIL;
            } else {
                SUCC;
                *cur = (*cur)->succ;
                // update cursor status
                if (!cursts) {
                    cursts = (*cur == lcur) ? 1 : -1;
                } else if (lcur == rcur) {
                    cursts = 0;
                }
            }
            break;

        case 'I':
            SUCC;
            GETCUR;
            insertbefore(*cur, getnbchar());
            break;

        case 'D':
            GETCUR;
            if (*cur == trailer) {
                // the cursor is already to the right of the rightmost character
                FAIL;
            } else {
                SUCC;
                // backup and move cursors
                Node* bak = *cur;
                if (lcur == bak) {
                    lcur = lcur->succ;
                }
                if (rcur == bak) {
                    rcur = rcur->succ;
                }
                del(bak);
                // update cursor status
                if (lcur == rcur) {
                    cursts = 0;
                }
            }
            break;

        case 'R':
            if (cursts == -1) {
                // left cursor is to the left of right cursor
                SUCC;
                Node* oldlcur = lcur;
                lcur = rcur->pred;
                reverse(oldlcur, rcur);
            } else {
                FAIL;
            }
            break;

        case 'S':
            show();
            break;

        default:
            break;

        }
    }
    return 0;
}

char getnbchar()
{
    char c;
    while (isspace(c = getchar()))
        ;
    return c;
}

void init()
{
    // initialize head
    head = pool;
    head->c = '\0';
    head->pred = NULL;
    head->succ = pool + 1;
    top = 1;

    // initialize body
    char c;
    while ((c = getchar()) != '\n') {
        pool[top].c = c;
        pool[top].pred = pool + (top - 1);
        pool[top].succ = pool + (top + 1);
        ++top;
    }

    // initialize trailer
    pool[top].c = '\0';
    pool[top].pred = pool + (top - 1);
    pool[top].succ = NULL;
    trailer = pool + (top++);
}

void insertbefore(Node* n, char c)
{
    Node* newnode = pool + (top++);
    newnode->c = c;
    newnode->succ = n;
    newnode->pred = n->pred;
    n->pred->succ = newnode;
    n->pred = newnode;
}

void del(Node* n)
{
    n->pred->succ = n->succ;
    n->succ->pred = n->pred;
}

void reverse(Node* lo, Node* hi)
{
    Node * curr, * tmp;
    curr = lo;
    while (curr != hi) {
        tmp = curr->pred;
        curr->pred = curr->succ;
        curr->succ = tmp;
        curr = curr->pred;
    }
    lo->succ->succ = hi->pred;
    hi->pred->pred = lo->succ;
    lo->succ = hi;
    hi->pred = lo;
}

void show()
{
    Node * curr = head->succ;
    while (curr != trailer) {
        putchar(curr->c);
        curr = curr->succ;
    }
    putchar('\n');
}
