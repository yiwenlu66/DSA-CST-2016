#include <stdio.h>
#include <string.h>
#include <stdint.h>
#define BUF_SZ (1 << 20)
#define MAXN (1000 + 1)   // include the '\0' at the end of the string
#define HASH_SZ (1 << 16)

char ibuf[BUF_SZ], obuf[BUF_SZ];

void minify(char* str);     // change str into its minimal representation
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

void minify(char* str)
{
    int l = (int)strlen(str);
    int i = 0, j = 1;
    while (j < l) {
        if (str[i] > str[j]) {
            i = j++;
        } else if (str[i] < str[j]) {
            ++j;
        } else {
            // str[i] == str[j]
            int k = 0;
            while (j + k < l && str[i + k] == str[j + k]) {
                ++k;
            }
            if (j + k == l) {
                break;
            }
            if (str[i + k] > str[j + k]) {
                i = j++;
            } else {
                ++j;
            }
        }
    }
    char tmp[MAXN];
    strcpy(tmp, str + i);
    for (j = l - 1; j >= l - i; --j) {
        str[j] = str[j - (l - i)];
    }
    for (j = 0; j < l - i; ++j) {
        str[j] = tmp[j];
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