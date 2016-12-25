#include <stdio.h>
#define MAXN 10000001

int users[MAXN];        // map user rank to position in code stack, 0 for N/A
int codestack[MAXN];    // stack for codes of online users, starts from 1, -1 for offline
int userstack[MAXN];    // stack for user verification, starts from 1
int top = 1;            // sentinel off the end of the stack
int nonline = 0;        // number of online users

int isonline(int a);

int main()
{
    int m;
    int a, c;
    char cmd;
    int result = 0;
    scanf("%d%d\n", &m, &m);    // ignore n
    while (--m >= 0) {
        scanf("%c", &cmd);
        switch(cmd) {
        case 'I':
            scanf("%d%d\n", &a, &c);
            if (isonline(a)) {
                codestack[users[a]] = c;
            } else {
                users[a] = top;
                userstack[top] = a;
                codestack[top++] = c;
                ++nonline;
            }
            break;
        case 'O':
            scanf("%d\n", &a);
            if (isonline(a)) {
                codestack[users[a]] = -1;
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
            scanf("%d\n", &a);
            result += isonline(a) ? codestack[users[a]] : -1;
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