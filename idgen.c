#include <stdio.h>
#include <stdint.h>
#include <time.h>

uint16_t seed = 0;
uint16_t smid[2];
uint8_t stid[2];

/*
* initialise random seed with current time in seconds, performed only once
* tid seed is time upper 8 bit and time lower 8 bit
* mid seed is time and time inverted
*/

void initialise()
{
    seed = time(0);
    stid[0] = seed;
    stid[1] = (seed >> 8);

    smid[0] = seed;
    smid[1] = ~seed;
}

uint8_t tidxorshift()
{
    if(seed == 0){
        initialise();
    }
    uint8_t x = stid[0];
	uint8_t y = stid[1];
	stid[0] = y;
	x ^= x << 3;
	stid[1] = x ^ y ^ (x >> 7) ^ (y >> 6);
	return stid[1] + y;

}

uint8_t midxorshift()
{
    if(seed == 0){
        initialise();
    }
    uint16_t x = smid[0];
	uint16_t y = smid[1];
	smid[0] = y;
	x ^= x << 13;
	smid[1] = x ^ y ^ (x >> 9) ^ (y >> 5);
	return smid[1] + y;

}