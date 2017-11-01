#include <cuda.h>
#include <cuda_runtime.h>
#include <stdio.h>
#include <stdint.h>
#include "../crack.h"

#define MAX_WORDSIZE 10
#define TOTAL_LETTERS 62

#define CHECK(fn) fn; \
	if ( ( cerr = cudaGetLastError() ) != cudaSuccess ) { \
		printf( "CUDA Error [%d] - '%s'" #fn "\n", __LINE__, cudaGetErrorString( cerr ) ); \
		exit(1); \
	}
__device__ void md5_vfy(unsigned char* data, uint length, uint *a1, uint *b1, uint *c1, uint *d1);
void md5_to_ints(unsigned char* md5, uint *r0, uint *r1, uint *r2, uint *r3);


__device__ char get_letter(int pos) {
	if(pos < 10) {
		return '0' + pos;
	}

	if(pos < 36) {
		return 'a' + pos - 10;
	}	

	return 'A' + pos - 36;
}

__global__ void thread_hierarchy(int len, uint32_t ha, uint32_t hb, uint32_t hc, uint32_t hd) {
	uint32_t search[4];
	search[0] = ha;
	search[1] = hb;
	search[2] = hc;
	search[3] = hd;

	int a = blockDim.x * blockIdx.x + threadIdx.x;
	int b = blockDim.y * blockIdx.y + threadIdx.y;
	int c = blockDim.z * blockIdx.z + threadIdx.z;

	if(a >= TOTAL_LETTERS || b >= TOTAL_LETTERS || c >= TOTAL_LETTERS) {
		return;
	}

	uint8_t start[MAX_WORDSIZE];
	for(int i = 0; i < MAX_WORDSIZE; i++) {
		start[i] = '0';
	}
	start[0] = get_letter(a);
	start[1] = get_letter(b);
	start[2] = get_letter(c);
	start[len] = 0;

	uint32_t computed[4];
	int checked = 0;
	for(;;) {	
		checked++;
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
			start[i]++;
			if(i < 3) {
//				printf("%d\n", checked);
				return;
			}
			if(start[i] == ':') {
				start[i] = 'A';
			} else if(start[i] == '[') {
				start[i] = 'a';
			} else if(start[i] == '{') {
				start[i] = '0';
				i--;

				continue;
			}
			break;
		}
	}
}

void cuda_crack(int wordLength, uint8_t *hash) {
	cudaError_t cerr;

	CHECK(cudaSetDeviceFlags(cudaDeviceScheduleBlockingSync));
	CHECK(cudaDeviceSetLimit(cudaLimitPrintfFifoSize, 1024*1024*80));

	uint32_t search[4];
	md5_to_ints(hash, &search[0], &search[1], &search[2], &search[3]);

	int len = 62; // total number of characters
	int threadsSize = 5;
	int blockSize = (len + threadsSize ) / threadsSize;
	printf("blocks(%d, %d, %d) threadsInBlock(%d, %d, %d)\n", blockSize, blockSize, blockSize, threadsSize, threadsSize, threadsSize);
	thread_hierarchy<<< dim3(blockSize, blockSize, blockSize), dim3(threadsSize, threadsSize, threadsSize)>>>(wordLength, search[0], search[1], search[2], search[3]);
	CHECK(1);

	CHECK(cudaDeviceSynchronize());
}
