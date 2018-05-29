#include <stdint.h>
#include <time.h>
static uint64_t s[16];
static int p;
//initialize state array with time since epoch in nanoseconds
void initialize()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    unsigned char i = 0;
    for (i=0;i<16;i++){
        s[i] = ts.tv_nsec;
    }
}
//https://en.wikipedia.org/wiki/Xorshift
uint64_t getRandom() {
	const uint64_t s0 = s[p++];
	uint64_t s1 = s[p &= 15];
	s1 ^= s1 << 31; // a
	s1 ^= s1 >> 11; // b
	s1 ^= s0 ^ (s0 >> 30); // c
	s[p] = s1;
	return s1 * (uint64_t)1181783497276652981;
}
