#include <stdio.h>
#include <ctype.h>
#define BUF_SZ 1 << 20
#define MAXNODE 8000002

#define FAIL printf("F\n");
#define SUCC printf("T\n");
#define GETCUR cur = (getnbchar() == 'L') ? &lcur : &rcur;

char ibuf[BUF_SZ], obuf[BUF_SZ];

typedef int ptr;    // use indices instead of pointers to save space

typedef struct Node {
    char c;
    ptr pred;
    ptr succ;
} Node;

// pre-allocate all nodes to increase efficiency
Node pool[MAXNODE];
int top;

char getnbchar();   // read non-blank character from stdin

// linked list
ptr head, trailer;
void init();    // read a string from stdin and initialize the list
void insertbefore(ptr n, char c);
void del(ptr n);
void reverse(ptr lo, ptr hi);
void show();


int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);

    init();
    ptr lcur = (pool + head)->succ, rcur = trailer; // point to the node that the cursor is to the LEFT of
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
        ptr* cur;

        switch (getnbchar()) {

        case '<':
            GETCUR;
            if ((pool + *cur)->pred == head) {
                // cursor is already to the left of the leftmost character
                FAIL;
            } else {
                SUCC;
                *cur = (pool + *cur)->pred;
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
                *cur = (pool + *cur)->succ;
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
                ptr bak = *cur;
                if (lcur == bak) {
                    lcur = (pool + lcur)->succ;
                }
                if (rcur == bak) {
                    rcur = (pool + rcur)->succ;
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
                ptr oldlcur = lcur;
                lcur = (pool + rcur)->pred;
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
    head = 0;
    pool->c = '\0';
    pool->pred = -1;
    pool->succ = 1;
    top = 1;

    // initialize body
    char c;
    while ((c = getchar()) != '\n') {
        pool[top].c = c;
        pool[top].pred = top - 1;
        pool[top].succ = top + 1;
        ++top;
    }

    // initialize trailer
    pool[top].c = '\0';
    pool[top].pred = top - 1;
    pool[top].succ = -1;
    trailer = top++;
}

void insertbefore(ptr n, char c)
{
    ptr newnode = top++;
    (pool + newnode)->c = c;
    (pool + newnode)->succ = n;
    (pool + newnode)->pred = (pool + n)->pred;
    (pool + (pool + n)->pred)->succ = newnode;
    (pool + n)->pred = newnode;
}

void del(ptr n)
{
    (pool + (pool + n)->pred)->succ = (pool + n)->succ;
    (pool + (pool + n)->succ)->pred = (pool + n)->pred;
}

void reverse(ptr lo, ptr hi)
{
    ptr curr, tmp;
    curr = lo;
    while (curr != hi) {
        tmp = (pool + curr)->pred;
        (pool + curr)->pred = (pool + curr)->succ;
        (pool + curr)->succ = tmp;
        curr = (pool + curr)->pred;
    }
    (pool + (pool + lo)->succ)->succ = (pool + hi)->pred;
    (pool + (pool + hi)->pred)->pred = (pool + lo)->succ;
    (pool + lo)->succ = hi;
    (pool + hi)->pred = lo;
}

void show()
{
    ptr curr = (pool + head)->succ;
    while (curr != trailer) {
        putchar((pool + curr)->c);
        curr = (pool + curr)->succ;
    }
    putchar('\n');
}
