#include <stdio.h>
#include <string.h>
#include <stdint.h>
#define BUF_SZ (1 << 20)
#define MAXN (1000 + 1)   // include the '\0' at the end of the string
#define HASH_SZ (1 << 16)

char ibuf[BUF_SZ], obuf[BUF_SZ];

void minify(char* str);     // change str into its minimal rotation
int hash(char* str);
inline void setval(int key, int val);
inline int getval(int key);   // 0 for unavailable

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);
    int m;
    char str[MAXN];
    scanf("%d%d", &m, &m);  // ignore n
    int i;
    for (i = 0; i < m; ++i) {
        scanf("%s", str);
        minify(str);
        int key = hash(str);
        int val = getval(key);
        printf("%d\n", val);
        if (!val) {
            setval(key, i + 1);
        }
    }
    return 0;
}

/* inspired by https://en.wikipedia.org/wiki/Lexicographically_minimal_string_rotation */
void minify(char* str)
{
    // concatenate str to itself
    char ss[MAXN << 1];
    int l = (int)strlen(str);
    int i;
    for (i = 0; i < l; ++i) {
        ss[i] = ss[i + l] = str[i];
    }

    // find minimal rotation (k)
    int f[MAXN << 1];
    memset(f, -1, (l << 1) * sizeof(int));
    int j, k = 0;
    for (j = 1; j < l << 1; ++j) {
        i = f[j - k - 1];
        while (i != - 1 && ss[j] != ss[k + i + 1]) {
            if (ss[j] < ss[k + i + 1]) {
                k = j - i - 1;
            }
            i = f[i];
        }
        if (ss[j] != ss[k + i + 1]) {
            if (ss[j] < ss[k]) {
                k = j;
            }
            f[j - k] = -1;
        } else {
            f[j - k] = i + 1;
        }
    }

    // change str into its minimal rotation
    for (i = 0; i < l; ++i) {
        str[i] = ss[i + k];
    }
}

char hashtable[HASH_SZ][MAXN];

#define FORWARD 0
#define BACKWARD 1

int hash(char* str)
{
    uint16_t h = 0;
    int i;
    for (i = 0; i < strlen(str); ++i) {
        h = (h << 3) | (h >> 13);
        h += str[i];
    }

    int direction = FORWARD;
    i = 1;
    while (strlen(hashtable[h]) && strcmp(hashtable[h], str)) {
        if (direction == FORWARD) {
            h += i * i;
            direction = BACKWARD;
        } else {
            h -= i * i;
            direction = FORWARD;
            ++i;
        }
    }
    strcpy(hashtable[h], str);
    return h;
}

int dict[HASH_SZ];

void setval(int key, int val)
{
    dict[key] = val;
}

int getval(int key)
{
    return dict[key];
}