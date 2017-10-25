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
__device__ void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest);

__device__ bool hash_equals(void* pa, void* pb, int len) {
	int *a = (int*) pa;
	int *b = (int*) pb;
	while(len--) {
		if(*a != *b) {
			return false;
		}
		a++;
		b++;
	}

	return true;
}

__global__ void thread_hierarchy(int len, uint8_t *search, char *table) {
	LETTERS

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

	uint8_t hash[16];
	for(;;) {	
		md5(start, len, hash);
		if(hash_equals(hash, search, sizeof(hash))) {
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

	uint8_t *gpu;
	CHECK(cudaMalloc(&gpu, 16));
	CHECK(cudaMemcpy(gpu, hash, 16, cudaMemcpyHostToDevice));

	char table[255];
	char* gpuTable;
	initTable(table);
	CHECK(cudaMalloc(&gpuTable, sizeof(table)));
	CHECK(cudaMemcpy(gpuTable, table, sizeof(table), cudaMemcpyHostToDevice));

	int len = sizeof(letters);
	int threadsSize = 10;
	int blockSize = (len + threadsSize )/ threadsSize;
	thread_hierarchy<<< dim3(blockSize, blockSize, blockSize), dim3(threadsSize, threadsSize, threadsSize)>>>(wordLength, gpu, gpuTable);
	CHECK(1);

	CHECK(cudaDeviceSynchronize());
}
