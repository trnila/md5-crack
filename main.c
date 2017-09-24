
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest);

int cmp(uint8_t *a, uint8_t *b) {
	return memcmp(a, b, 16 * sizeof(uint8_t)) == 0;
}

// python
// import string;  string.ascii_letters + string.digits
const char *letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

int crack(char *str, int idx, int len, uint8_t *search) {
	if (idx < (len - 1)) {
		for (int c = 0; c < 62; ++c) {
			str[idx] = letters[c];

			if(crack(str, idx + 1, len, search)) {
				return 1;
			}
		}
	} else {
		for (int c = 0; c < 62; ++c) {
			str[idx] = letters[c];

			uint8_t hash[16];
			md5((uint8_t*)str, len, hash);
			if(cmp(search, hash)) {
				printf("Found: %s\n", str);
				return 1;
			}
		}
	}

	return 0;
}


int main(int argc, char **argv) {
	if(argc != 3) {
		printf("Usage: %s hash letter\n", argv[0]);
		exit(1);
	}

	uint8_t search[16];	
	for (int i = 0; i < 16; i++) {
		sscanf(argv[1] + 2*i, "%2x", &search[i]);
	}


	int size = atoi(argv[2]);
	printf("Going to crack %d length...\n", size);

	char str[size + 1];
	str[size] = 0;
	crack(str, 0, size, search);


	return 0;
}
