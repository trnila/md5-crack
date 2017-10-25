#include <cuda.h>
#include <cuda_runtime.h>
#include <stdio.h>
#include <stdint.h>
#include "../crack.h"

#define LETTERS const char letters[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
//#define LETTERS const char letters[] = "0123456789";
#define MAX_WORDSIZE 10

#define CHECK(fn) fn; \
	if ( ( cerr = cudaGetLastError() ) != cudaSuccess ) { \
		printf( "CUDA Error [%d] - '%s'" #fn "\n", __LINE__, cudaGetErrorString( cerr ) ); \
		exit(1); \
	}
__device__ void md5_vfy(unsigned char* data, uint length, uint *a1, uint *b1, uint *c1, uint *d1);
void md5_to_ints(unsigned char* md5, uint *r0, uint *r1, uint *r2, uint *r3);
void initTable(char* table) {
    for(int i = 0; i < 254; i++) {
	    table[i] = i + 1;
    }
    table['9'] = 'A';
    table['Z'] = 'a';
    table['z'] = '0';
}

__global__ void thread_hierarchy(int len, char *table, uint32_t ha, uint32_t hb, uint32_t hc, uint32_t hd) {
	LETTERS
	uint32_t search[4];
	search[0] = ha;
	search[1] = hb;
	search[2] = hc;
	search[3] = hd;

	int a = blockDim.x * blockIdx.x + threadIdx.x;
	int b = blockDim.y * blockIdx.y + threadIdx.y;
	int c = blockDim.z * blockIdx.z + threadIdx.z;

	if(a >= sizeof(letters) || b >= sizeof(letters) || c >= sizeof(letters)) {
		return;
	}

	uint8_t start[MAX_WORDSIZE];
	for(int i = 0; i < MAX_WORDSIZE; i++) {
		start[i] = letters[0];
	}
	start[0] = letters[a];
	start[1] = letters[b];
	start[2] = letters[c];
	start[len] = 0;

	uint32_t computed[4];
	for(;;) {	
		md5_vfy(start, len, &computed[0], &computed[1], &computed[2], &computed[3]);
		if(computed[0] == search[0] && computed[1] == search[1] && computed[2] == search[2] && computed[3] == search[3]) {
			printf("Hash found %s\n", start);
			__threadfence();
			asm("trap;");
			return;
		}

		int i = len - 1;
		for(;;) {
			// increment last char
			start[i] = table[start[i]];

			// character not overflowed to start, break and test new combination
			if(start[i] != letters[0]) {
				break;
			}

			i--;

			if(i < 3) {
				return;
			}
		}
	}
}

void cuda_crack(int wordLength, uint8_t *hash) {
	LETTERS

	cudaError_t cerr;

	//CHECK(cudaDeviceSetLimit(cudaLimitPrintfFifoSize, 1024*1024*80));

	uint32_t search[4];
	md5_to_ints(hash, &search[0], &search[1], &search[2], &search[3]);

	char table[255];
	char* gpuTable;
	initTable(table);
	CHECK(cudaMalloc(&gpuTable, sizeof(table)));
	CHECK(cudaMemcpy(gpuTable, table, sizeof(table), cudaMemcpyHostToDevice));

	int len = sizeof(letters);
	int threadsSize = 5;
	int blockSize = (len + threadsSize ) / threadsSize;
	printf("blocks(%d, %d, %d) threadsInBlock(%d, %d, %d)\n", blockSize, blockSize, blockSize, threadsSize, threadsSize, threadsSize);
	thread_hierarchy<<< dim3(blockSize, blockSize, blockSize), dim3(threadsSize, threadsSize, threadsSize)>>>(wordLength, gpuTable, search[0], search[1], search[2], search[3]);
	CHECK(1);

	CHECK(cudaDeviceSynchronize());
}
