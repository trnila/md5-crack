#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../crack.h"

void cuda_crack(int len, uint8_t *hash);

int main(int argc, char **argv) {
	if(argc < 3) {
		printf("Usage: %s hash passsize\n", argv[0]);
		exit(1);
	}

	uint8_t search[16];
	parseHash(argv[1], search);
	int len = atoi(argv[2]);

	cuda_crack(len, search);
	return 0;
}
