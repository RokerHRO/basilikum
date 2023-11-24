#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "basilikum.h"
#include <unistd.h>




int main(int argc, char** argv)
{
	basilikum_init();
	unsigned long long secret = 0x123456789;
	int c = 0;
	
	while( (c = getopt(argc, argv, "hs:")) != -1)
	{
		switch(c)
		{
			case 's':
				secret = strtoull(optarg, NULL, 0);
				break;
			case 'h':
			case '?':
				fprintf(stderr, "Usage: basilikum [-s secret]  text ...\n\n");
				return 1;
		}
	}
	
	for(int a=optind; a<argc; ++a)
	{
		char* f = basilikum_encode(argv[a], secret);
		printf("\n#%02u \"%s\" (len=%zu) -> {%s} (len=%zu) \n\n", a, argv[a], strlen(argv[a]), f, strlen(f));
		
		char* uf = basilikum_decode(f, secret);
		
		printf("\n    \"%s\" (len=%zu) \n\n", uf, strlen(uf));
		
		
		free(uf);
		free(f);
	}
}
