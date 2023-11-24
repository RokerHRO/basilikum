#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


static const char* const alphabet =
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"-.";

static signed char unalpha[256];

typedef unsigned long long ULL;


void basilikum_init()
{
	for(unsigned u=0; u<256; ++u)
		unalpha[u] = -1;
	
	for(unsigned u=0; u<=62; ++u)
	{
		unalpha[ (unsigned char)alphabet[u] ] = u;
	}
}

static
int sidx(const char* input, size_t max_len, size_t idx)
{
	const int c = idx<max_len ? input[idx] : 32;
	return (c<32 || c>126) ? 94 : c-32;
}

static
size_t base62(char* out, uint64_t coll, unsigned length)
{
	size_t written = 0;
	while(length){
		unsigned rem = coll % 62;
		coll = coll / 62;
		*out = alphabet[rem];
		
		++out;
		++written;
		--length;
	};
	
	return written;
}


char* basilikum_encode(const char* input, const unsigned long long secret)
{
	const size_t input_len = strlen(input);
	const size_t input_pkt = input_len/9;
	const size_t input_rem = input_len%9;
	const size_t output_len = (input_pkt * 10) + (input_rem ? input_rem+1u : 0u);
	
	char* output = calloc(1, output_len + 1);
//	memset(output, '#', output_len);
	char* optr = output;
	unsigned long long rs = secret;
	unsigned iidx = 0;
	uint64_t factor = 1;
	uint64_t coll = 0;
	for(size_t i=0; i<input_len; ++i)
	{
		const int si = sidx(input, input_len, i);
		coll += (si + (rs%89))%96 * factor;
//		coll += (si * factor);
		
		rs = rs / 89;
		if(rs<101)
			rs += secret;
		
		factor *= 96;
		if(++iidx == 9)
		{
			optr  += base62(optr, coll, iidx+1);
			iidx   = 0;
			factor = 1;
			coll   = 0;
		}
	}
	
	if(iidx)
	{
		base62(optr, coll, iidx+1);
	}
	
	return output;
};


static
void unbase96(char* out, uint64_t coll, unsigned iidx, unsigned long long* rs, const unsigned long long secret)
{
	while(--iidx)
	{
		*out = ' '+ (96 + (coll % 96) - *rs%89)%96;
		coll = coll/96;
		++out;
		
		*rs = *rs / 89;
		if(*rs<101)
			*rs += secret;
	}
}


char* basilikum_decode(const char* input, const unsigned long long secret)
{
	const size_t input_len = strlen(input);
	const size_t input_pkt = input_len/10;
	const size_t input_rem = input_len%10;
	const size_t output_len = (input_pkt * 9) + (input_rem ? input_rem-1u : 0u);
	
	char* output = calloc(1, output_len + 1);
	char* optr = output;
	unsigned long long rs = secret;
	unsigned iidx = 0;
	uint64_t factor = 1;
	uint64_t coll = 0;
	for(size_t i=0; i<input_len; ++i)
	{
		const int val = unalpha[ (unsigned char) input[i] ];
		if(val<0)
		{
			printf("!ERROR!\n"); fflush(stdout);
			continue;
		}
		coll += val*factor;
		++iidx;
		factor *=62;
		if(iidx==10)
		{
			unbase96(optr, coll, iidx, &rs, secret);
			optr += iidx-1;
			assert(optr-output <= output_len);
			iidx = 0;
			factor = 1;
			coll=0;
		}
	}
	
	if(iidx)
	{
		unbase96(optr, coll, iidx, &rs, secret);
		optr += iidx-1;
		assert(optr-output <= output_len);
	}
	
	return output;
}
