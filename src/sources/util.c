#include "util.h"

static void mul128(u64 x, u64 y, u64 *restrict l, u64 *restrict h)
{
#if !defined(__GNUC__)
        const u64 ah = x >> 32, al = x & ~0u, bh = y >> 32, bl = y & ~0u,
              v0 = ah * bh, v1 = al * bh, v2 = ah * bl, v3 = al * bl,
              v4 = v2 + (v3 >> 32) + v1;
        *l = ((v4 & ~0u) << 32) + (v3 & ~0u);
        *h = v0 + ((u64)(v4 < v1) << 32) + (v4 >> 32);
#else
        const __uint128_t r = (__uint128_t)x * y;
        *l = (u64)r;
        *h = (u64)(r >> 64);
#endif
}

static void wymum(u64 *restrict a, u64 *restrict b)
{
        mul128(*a, *b, b, a);
}

static u64 wymix(u64 a, u64 b)
{
        wymum(&a, &b);
        return a ^ b;
}

static u64 wyr3(const u8 *p, usz k)
{
        return (((u64)p[0]) << 16) | (((u64)p[k>>1]) << 8) | p[k-1];
}

static u64 wyr4(const u8 *p)
{
        u32 v;
        memcpy(&v, p, 4);
        return v;
}

static u64 wyr8(const u8 *p)
{
        u64 v;
        memcpy(&v, p, 8);
        return v;
}

u64 hash(const void *p, usz sz, u64 seed)
{
        static const u64 k[4] = {
                0xA0761D6478BD642Full, 0xE7037ED1A0B428DBull,
                0x8EBC6AF09C88C6E3ull, 0x589965CC75374CC3ull
        };

        const u8 *q = (const u8*)p;
        seed ^= wymix(seed ^ k[0], k[1]);

        u64 a, b;
        if (LIKELY(sz <= 16)) {
                if (LIKELY(sz >= 4)) {
                        a = (wyr4(q) << 32) | wyr4(q + ((sz >> 3) << 2));
                        b = (wyr4(q + sz - 4) << 32) | wyr4(q + sz - 4 - ((sz >> 3) << 2));
                } else if (LIKELY(sz > 0)) {
                        a = wyr3(q, sz), b = 0;
                } else {
                        a = b = 0;
                }
        } else {
                usz i = sz; 
                if (UNLIKELY(i > 48)) {
                        u64 s1 = seed, s2 = seed;
                        do {
                                seed = wymix(wyr8(q) ^ k[1], wyr8(q + 8) ^ seed);
                                s1 = wymix(wyr8(q + 16) ^ k[2], wyr8(q + 24) ^ s1);
                                s2 = wymix(wyr8(q + 32) ^ k[3], wyr8(q + 40) ^ s2);
                                q += 48, i -= 48;
                        } while (LIKELY(i > 48));
                        seed ^= s1 ^ s2;
                }
                while (UNLIKELY(i > 16)) {
                        seed = wymix(wyr8(q) ^ k[1], wyr8(q + 8) ^ seed);
                        i -= 16, q += 16;
                }
                a = wyr8(q + i - 16), b = wyr8(q + i - 8);
        }

        a ^= k[1], b ^= seed;
        wymum(&a, &b);

        return wymix(a ^ k[0] ^ sz, b ^ k[1]);
}

u32 getnprocs(void)
{
#ifdef _WIN32
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
#else
	return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}
