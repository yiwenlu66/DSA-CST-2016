#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#define BUF_SZ (1 << 20)

char ibuf[BUF_SZ], obuf[BUF_SZ];

typedef struct {
    uint64_t bits;
    uint8_t len;
} String;

String getstr();  // read a binary string from stdin

/***************************
 * Trie interface
 ***************************/
inline uint8_t insert(String str);   // insert the binary string into the trie and return the new winner

int main()
{
    setvbuf(stdin, ibuf, _IOFBF, BUF_SZ);
    setvbuf(stdout, obuf, _IOFBF, BUF_SZ);
    int n;
    scanf("%d", &n);
    int start = 0, i;
    uint8_t winner;
    for (i = 0; i < n; ++i) {
        if (i == 0) {
            // first time
            winner = insert(getstr());
        } else if (winner != insert(getstr())) {
            // winner changed
            printf(winner ? "Eve" : "Adam");
            printf(" %d %d\n", start + 1, i);
            winner = !winner;
            start = i;
        }
    }
    printf(winner ? "Eve" : "Adam");
    printf(" %d %d\n", start + 1, n);
    return 0;
}

String getstr()
{
    uint64_t bits = 0;
    uint8_t len;
    int c;
    while ((c = getchar()) < '0' || c > '1')
        ;
    for (len = 0; ; ++len) {
        switch (c) {
            case '0':
                bits <<= 1;
                break;
            case '1':
                bits = (bits << 1) + 1;
                break;
            default:
                return (String){ .bits = bits, .len = len };
        }
        c = getchar();
    }
}

/***************************
 * Trie implementation
 ***************************/

#define MAXNODE 5050000
#define MAX_BITS_PER_NODE 13

/*
 * Node structure ([hi, lo]: from bit hi to bit lo):
 *  - [63, 41]: address of left child, all 0 for non-existence
 *  - [40, 18]: address of right child, all 0 for non-existence
 *  - [17, 14]: length of stored bits
 *  - [13,  1]: stored bits
 *  - [0 ,  0]: winner (0 for Adam, 1 for Eve)
 */
uint64_t nodes[MAXNODE];

typedef uint32_t addr;
uint32_t n_alloc = 1;   // # allocated nodes (nodes[0] is preserved for root)
addr alloc();   // allocate a new node and return the address of the new node

inline uint64_t _getbits(uint64_t n, uint8_t hi, uint8_t lo);  // get [hi, lo] of n
inline void _setbits(uint64_t* dst, uint8_t hi, uint8_t lo, uint64_t src);  // copy src to [hi, lo] of dst

// setters return the value that has been set
inline addr getlc(addr);
inline addr setlc(addr parent, addr lc);
inline addr getrc(addr);
inline addr setrc(addr parent, addr rc);
inline uint8_t getlen(addr);
inline uint8_t setlen(addr node, uint8_t len);
inline uint16_t getbits(addr);
inline uint16_t setbits(addr node, uint16_t bits);
inline uint8_t getwinner(addr);
inline uint8_t setwinner(addr node, uint8_t winner);

uint8_t prefix(String s1, String s2);     // get common prefix length
int split(addr node, uint8_t pos);  // return 0 if the node is split into parent and lc; 1 for parent and rc

// insert starting from hi bit
inline uint8_t _insert(addr root, String str, uint8_t hi);
uint8_t _insert_l(addr root, String str, uint8_t hi);
uint8_t _insert_r(addr root, String str, uint8_t hi);
uint8_t _insert_new(addr new, String str, uint8_t hi);
uint8_t _insert_existing(addr existing, String str, uint8_t hi);

uint8_t insert(String str)
{
    if (str.len) {
        return _insert(0, str, str.len - 1);
    }
    return getwinner(0);
}

uint8_t _insert(addr root, String str, uint8_t hi)
{
    return setwinner(root, _getbits(str.bits, hi, hi) ? _insert_r(root, str, hi) : _insert_l(root, str, hi));
}

uint8_t _insert_l(addr root, String str, uint8_t hi)
{
    uint8_t l_winner;
    addr lc = getlc(root), rc = getrc(root);
    if (!lc) {
        lc = alloc();
        setlc(root, lc);
        l_winner = _insert_new(lc, str, hi);
    } else {
        l_winner = _insert_existing(lc, str, hi);
    }
    if (!rc) {
        return l_winner;
    }
    uint8_t r_winner = getwinner(rc);
    return setwinner(root, ((str.len - hi) & 1) ? (l_winner && r_winner) : (l_winner || r_winner));
}

uint8_t _insert_r(addr root, String str, uint8_t hi)
{
    uint8_t r_winner;
    addr lc = getlc(root), rc = getrc(root);
    if (!rc) {
        rc = alloc();
        setrc(root, rc);
        r_winner = _insert_new(rc, str, hi);
    } else {
        r_winner = _insert_existing(rc, str, hi);
    }
    if (!lc) {
        return r_winner;
    }
    uint8_t l_winner = getwinner(lc);
    return setwinner(root, ((str.len - hi) & 1) ? (l_winner && r_winner) : (l_winner || r_winner));
}

uint8_t _insert_new(addr new, String str, uint8_t hi)
{
    if (hi < MAX_BITS_PER_NODE) {
        setbits(new, (uint16_t)_getbits(str.bits, hi, 0));
        setlen(new, hi + 1);
    } else {
        uint16_t bits = (uint16_t)_getbits(str.bits, hi, hi - MAX_BITS_PER_NODE + 1);
        hi -= MAX_BITS_PER_NODE;
        setbits(new, bits);
        setlen(new, MAX_BITS_PER_NODE);
        addr c = alloc();
        if (_getbits(str.bits, hi, hi)) {
            setrc(new, c);
        } else {
            setlc(new, c);
        }
        _insert_new(c, str, hi);
    }
    return setwinner(new, !(str.len & 1));
}

uint8_t _insert_existing(addr existing, String str, uint8_t hi)
{
    uint8_t existing_len = getlen(existing);
    uint16_t existing_bits = getbits(existing);
    uint8_t prefix_len = prefix((String){ .len = existing_len, .bits = (uint64_t)existing_bits },
                                (String){ .len = hi + 1, .bits = str.bits});
    if (hi + 1 == prefix_len) {
        // new node can be "absorbed" by existing node
        return getwinner(existing);
    }
    // wipe prefix
    hi -= prefix_len;
    if (existing_len == prefix_len) {
        return _insert(existing, str, hi);
    }
    uint8_t l_winner, r_winner;
    if (!split(existing, prefix_len)) {
        // existing node split into parent and left child, insert new chain as right child
        addr rc = alloc();
        setrc(existing, rc);
        r_winner = _insert_new(rc, str, hi);
        l_winner = getwinner(getlc(existing));
    } else {
        // existing node split into parent and right child, insert new chain as left child
        addr lc = alloc();
        setlc(existing, lc);
        l_winner = _insert_new(lc, str, hi);
        r_winner = getwinner(getrc(existing));
    }
    return setwinner(existing, ((str.len - hi) & 1) ? (l_winner && r_winner) : (l_winner || r_winner));
}

addr alloc()
{
    if (n_alloc == MAXNODE) {
        // memory overflow
        exit(-1);
    }
    return n_alloc++;
}


addr getlc(addr p)
{
    return (addr)(_getbits(nodes[p], 63, 41));
}

addr setlc(addr p, addr lc)
{
    _setbits(nodes + p, 63, 41, (uint64_t)lc);
    return lc;
}

addr getrc(addr p)
{
    return (addr)(_getbits(nodes[p], 40, 18));
}

addr setrc(addr p, addr rc)
{
    _setbits(nodes + p, 40, 18, (uint64_t)rc);
    return rc;
}

uint8_t getlen(addr p)
{
    return (uint8_t)(_getbits(nodes[p], 17, 14));
}

uint8_t setlen(addr p, uint8_t l)
{
    _setbits(nodes + p, 17, 14, (uint64_t)l);
    return l;
}

uint16_t getbits(addr p)
{
    return (uint16_t)(_getbits(nodes[p], 13, 1));
}

uint16_t setbits(addr p, uint16_t b)
{
    _setbits(nodes + p, 13, 1, (uint64_t)b);
    return b;
}

uint8_t getwinner(addr p)
{
    return (uint8_t)(_getbits(nodes[p], 0, 0));
}

uint8_t setwinner(addr p, uint8_t w)
{
    _setbits(nodes + p, 0, 0, (uint64_t)w);
    return w;
}

uint64_t _getbits(uint64_t n, uint8_t hi, uint8_t lo)
{
    return (n >> lo) & ~(~0ULL << (hi - lo + 1));
}

void _setbits(uint64_t* dst, uint8_t hi, uint8_t lo, uint64_t src)
{
    *dst = ((*dst) & ~(~(~0ULL << (hi - lo + 1)) << lo)) + (src << lo);
}

uint8_t prefix(String s1, String s2)
{
    uint8_t l = max(s1.len, s2.len);
    s1.bits <<= (l - s1.len);
    s2.bits <<= (l - s2.len);
    uint64_t xor = s1.bits ^ s2.bits;
    uint8_t i, m = 0;
    for (i = 1; i <= l; ++i, xor >>= 1) {
        if (xor & 1) {
            m = i;
        }
    }
    return min(l - m, min(s1.len, s2.len));
}

int split(addr node, uint8_t pos)
{
    uint16_t oldbits = getbits(node);
    uint8_t oldlen = getlen(node);
    uint8_t p_len = pos, c_len = oldlen - pos;
    uint16_t p_bits = oldbits >> c_len;
    uint16_t c_bits = (uint16_t)_getbits((uint64_t)oldbits, c_len - 1, 0);
    setlen(node, p_len);
    setbits(node, p_bits);
    addr c = alloc();
    setlen(c, c_len);
    setbits(c, c_bits);
    setlc(c, getlc(node));
    setrc(c, getrc(node));
    setwinner(c, getwinner(node));
    if (c_bits >> (c_len - 1)) {
        setrc(node, c);
        setlc(node, 0);
        return 1;
    } else {
        setlc(node, c);
        setrc(node, 0);
        return 0;
    }
}