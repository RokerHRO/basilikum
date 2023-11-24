#include "basilikum.h"

#include <gtest/gtest.h>

TEST(Basili, Simple)
{
	EXPECT_EQ(3, strlen("foo"));
}

const unsigned long long BASIL_SECRET = 20230622123400;

char input[16] = {'\0' };

int compare(const char* orig, const char* deco)
{
	const int c = strcmp(orig, deco);
	if(c)
	{
		printf(" ERROR \"%s\" != \"%s\" c=%d!\n", orig, deco, c);
		const size_t l1 = strlen(orig);
		const size_t l2 = strlen(deco);
		const size_t m = l1<l2 ? l1 : l2;
		printf("\t l1=%zu  l2=%zu  m=%zu\n", l1, l2, m);
		for(size_t i=0; i<m; ++i)
		{
			printf("\t[%zu]  0x%02hhx 0x%02hhx %s\n", i, orig[i], deco[i], (orig[i]==deco[i] ? "==" : "!!") );
		}
		
		if(l1<l2)
		{
			for(size_t i=l1 ; i<l2; ++i)
			{
				printf("\t[%zu]  ---- 0x%02hhx\n", i, deco[i]);
			}
		}else{
			for(size_t i=l2 ; i<l1; ++i)
			{
				printf("\t[%zu]  0x%02hhx ----\n", i, orig[i]);
			}
		}
	}else{
//		printf(" OK\n");
	}
	return c;
}

int brute_force(int len)
{
	for(char c='!'; c<='~'; ++c)
	{
		input[len] = c;
		
		if(len==0)
		{
			char* e = basilikum_encode(input, BASIL_SECRET);
			char* d = basilikum_decode(e, BASIL_SECRET);
			const int c = compare(input,d);
			if(c)
			{
				printf("«%s» -> «%s» -> «%s»  ", input, e, d);
				return c;
			}
		}else{
			brute_force(len-1);
		}
	}
	return 0;
}


TEST(Basili, Example_y)
{
	const char* input = "y";
	const char* e = basilikum_encode(input, BASIL_SECRET);
	const char* d = basilikum_decode(e, BASIL_SECRET);
	ASSERT_FALSE(compare(input,d));
}

/*
TEST(Basili, BruteForce)
{
	for(int len=0; len<15; ++len)
	{
		fprintf(stderr, "Len=%u. \n", len);
		ASSERT_FALSE(brute_force(len));
	}
}
*/

// from Wikipedia:
uint64_t kiss64()
{
	// interner Zustand
	static uint64_t x = 1066149217761810ULL;
	static uint64_t y = 362436362436362436ULL;
	static uint64_t z = 1234567890987654321ULL;
	static uint64_t c = 123456123456123456ULL;
	
	// Linearer Kongruenzgenerator
	x = 6906969069ULL * x + 1234567;
	
	// Xorshift
	y ^= y << 13;
	y ^= y >> 17;
	y ^= y << 43;
	
	// Multiply-with-carry
	const uint64_t t = (z << 58) + c;
	c = z >> 6;
	z += t;
	c += z < t;
	
	return x + y + z;
}

double dRand()
{
	return (long double)kiss64() / 18446744073709551616.0L;
}

std::string random_string()
{
	const unsigned len = 10 + kiss64()%10;
	std::string s;
	s.reserve(len);
	for(unsigned u=0; u<len; ++u)
	{
		s += char('!' + dRand()*94);
	}
	return s;
}


TEST(Basili, RandString)
{
	for(unsigned u=0; u<100; ++u)
	{
		const std::string input = random_string();
		char* e = basilikum_encode(input.c_str(), BASIL_SECRET * 3);
		char* f = basilikum_encode(input.c_str(), 0);
		char* d = basilikum_decode(e, BASIL_SECRET*3);
		printf("%20s» -> «%-22s» %s\n", input.c_str(), e, f);
		ASSERT_FALSE(compare(input.c_str(),d));
		free(d);
		free(f);
		free(e);
	}
}


TEST(Basili, MinMaxString)
{
	std::string input_min, input_max;
	
	for(unsigned u=0; u<100; ++u)
	{
		input_min += '!';
		char* e_min = basilikum_encode(input_min.c_str(), BASIL_SECRET * 3);
		char* d_min = basilikum_decode(e_min, BASIL_SECRET*3);
		printf("%20s» -> «%-22s»\n", input_min.c_str(), e_min);
		ASSERT_FALSE(compare(input_min.c_str(),d_min));
		free(d_min);
		free(e_min);
		
		input_max += '~';
		char* e_max = basilikum_encode(input_max.c_str(), BASIL_SECRET * 3);
		char* d_max = basilikum_decode(e_max, BASIL_SECRET*3);
		printf("%20s» -> «%-22s» \n", input_max.c_str(), e_max);
		ASSERT_FALSE(compare(input_max.c_str(),d_max));
		
		free(d_max);
		free(e_max);
	}
}


int main(int argc, char** argv)
{
	basilikum_init();
	
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
