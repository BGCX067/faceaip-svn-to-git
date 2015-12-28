/**
 *author sunzq
 *	2012-07-12
 */

#include "app.h"

static unsigned long long swap64(unsigned long long _x)  
{
    /* this is __bswap64 from:
    * $FreeBSD: src/sys/i386/include/endian.h,v 1.41$
    */
    return ((_x >> 56) | ((_x >> 40) & 0xff00) | ((_x >> 24) & 0xff0000) |
            ((_x >> 8) & 0xff000000) | ((_x << 8) & ((unsigned long long)0xff << 32)) |
          ((_x << 24) & ((unsigned long long)0xff << 40)) |
           ((_x << 40) & ((unsigned long long)0xff << 48)) | ((_x << 56)));
}


unsigned long long hton64(const unsigned long long ho)  
{
    if (ntohs(0x1234) == 0x1234)
        return ho;  
    else 
        return swap64(ho);
}

unsigned long long ntoh64(const unsigned long long no)
{
    return hton64(no);
}

static int64_t base_swap(const u_char bs[], int len)
{
	int64_t s =0; 
	int i;
	int count = len - 1;
	for(i = 0; i < len; i++){
		int64_t t = bs[i]&0xFF;
		t <<= 8*(count - i);
		s |= t;
	}
	return s;
}


int64_t btoll(const u_char bs[], int len)
{
	return base_swap(bs, len);
}

int btoi(const u_char bs[], int len)
{
	return base_swap(bs, len);
}

short btos(const u_char bs[], int len)
{
	return base_swap(bs, len);
}

