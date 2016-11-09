#include <stdio.h>
#include <stdint.h>
#define BUF_SZ (1 << 20)
#define LINEBUF_SZ 25
#define MAXSTOCK 1000000

char ibuf[BUF_SZ], obuf[BUF_SZ];

// queap, inspired by slides
int queue[MAXSTOCK];
int maxqueue[MAXSTOCK];
int front, rear;

void enqueue(int val);
int dequeue();  // return the value dequeued from maxqueue
int getmax();

int isinrange(int i);   // tell whether i is a valid index in queue
void moveback(int* i);
void moveforward(int* i);

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);
    int N;
    scanf("%d", &N);
    getchar();  // consume '\n'
    int i;
    char linebuf[LINEBUF_SZ];
    int dt, val;
    uint64_t sum = 0;
    for (i = 0; i < N << 1; ++i) {
        fgets(linebuf, LINEBUF_SZ, stdin);
        val = -1;
        sscanf(linebuf, "%d%d", &dt, &val);
        if (val != -1) {
            sum += (uint64_t)dt * (uint64_t)getmax();
            enqueue(val);
        } else {
            sum += (uint64_t)dt * (uint64_t)dequeue();
        }
    }
    printf("%lld\n", sum);
    return 0;
}

void enqueue(int val)
{
    queue[front] = maxqueue[front] = val;
    int i;
    for (i = front, moveforward(&front); isinrange(i) && maxqueue[i] <= val; moveback(&i)) {
        maxqueue[i] = val;
    }
}

int dequeue()
{
    int ret = maxqueue[rear];
    moveforward(&rear);
    return ret;
}

int getmax()
{
    return maxqueue[rear];
}

int isinrange(int i)
{
    if (rear < front) {
        return rear <= i && i < front;
    }
    return rear <= i || i < front;
}

void moveback(int* i)
{
    --(*i);
    if (*i < 0) {
        *i += MAXSTOCK;
    }
}

void moveforward(int* i)
{
    ++(*i);
    if (*i >= MAXSTOCK) {
        *i -= MAXSTOCK;
    }
}
