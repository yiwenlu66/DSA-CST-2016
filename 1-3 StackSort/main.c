#include <stdio.h>
#define MAXN 2000000
#define BUF_SZ 1 << 22

int seen[MAXN]; // boolean values, whether the number has been read
int stack[MAXN];
int top = 0;

char ibuf[BUF_SZ], obuf[BUF_SZ];

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);
    int n, max_unseen, curr;
    scanf("%d", &n);
    max_unseen = n;
    while (n-- > 0) {
        scanf("%d", &curr);
        seen[curr] = 1;
        if (curr == max_unseen) {
            printf("%d ", curr);
            while (seen[max_unseen]) {
                --max_unseen;
            }
            while (top > 0 && stack[top - 1] > max_unseen) {
                printf("%d ", stack[--top]);
            }
        } else {
            stack[top++] = curr;
        }
    }
    while (top > 0) {
        printf("%d ", stack[--top]);
    }
    printf("\n");
    return 0;
}
