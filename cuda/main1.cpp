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

	int len = atoi(argv[2]);
	cuda_crack(len, (uint8_t*) argv[1]);
	return 0;
}
