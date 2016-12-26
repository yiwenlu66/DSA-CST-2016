#include <stdio.h>
#include <stdint.h>
#define BUF_SZ (1 << 20)
#define MOD 520019

char ibuf[BUF_SZ];

int users[MOD];            // map user rank to position in code stack, 0 for N/A
int codestack[MOD + 1];    // stack for codes of online users, starts from 1, -1 for offline
int userstack[MOD + 1];    // stack for user verification, starts from 1
int top = 1;               // sentinel off the end of the stack
int nonline = 0;           // number of online users

int isonline(int a);

int hash(uint64_t x);      // return an unused hash value in [0, MOD)

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);

    int m;
    uint64_t a;
    int c;
    char cmd;
    int result = 0;
    scanf("%d%d\n", &m, &m);    // ignore n
    while (--m >= 0) {
        scanf("%c", &cmd);
        switch(cmd) {
        case 'I':
            scanf("%lld%d\n", &a, &c);
            if (isonline(hash(a))) {
                codestack[users[hash(a)]] = c;
            } else {
                users[hash(a)] = top;
                userstack[top] = hash(a);
                codestack[top++] = c;
                ++nonline;
            }
            break;
        case 'O':
            scanf("%lld\n", &a);
            if (isonline(hash(a))) {
                codestack[users[hash(a)]] = -1;
                --nonline;
            }
            break;
        case 'C':
            scanf("\n");
            top = 1;
            nonline = 0;
            break;
        case 'N':
            scanf("\n");
            result += nonline;
            break;
        case 'Q':
            scanf("%lld\n", &a);
            result += isonline(hash(a)) ? codestack[users[hash(a)]] : -1;
            break;
        default:
            break;
        }
    }
    printf("%d\n", result);
    return 0;
}


int isonline(int a)
{
    return !((users[a] >= top) || (!users[a]) || (codestack[users[a]] == -1) || (userstack[users[a]] != a));
}

uint64_t hashtable[MOD];    // 0 for vacancy

#define FORWARD 0
#define BACKWARD 1

int hash(uint64_t x)
{
    ++x;    // make sure x is positive
    int pos = (int)(x % MOD);
    int direction = FORWARD;
    uint64_t i = 1;
    while (hashtable[pos] && hashtable[pos] != x) {
        if (direction == FORWARD) {
            pos = (int)(((uint64_t)pos + i * i) % MOD);
            direction = BACKWARD;
        } else {
            pos = (int)(((uint64_t)pos + i * MOD - i * i) % MOD);
            direction = FORWARD;
            ++i;
        }
    }
    hashtable[pos] = x;
    return pos;
}