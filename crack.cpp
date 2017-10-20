#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <stdint.h>
#include <stdlib.h>

#define FOUND 1
#define NOT_FOUND 0

void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest);

void initTable(char* table) {
    for(int i = 0; i < 254; i++) {
	    table[i] = i + 1;
    }
    table['9'] = 'A';
    table['Z'] = 'a';
    table['z'] = '0';
}

int crack(char* str, int len, char stop, uint8_t* search) {
	uint8_t hash[16];
	char table[255];
	initTable(table);
	//printf("%c %c %d\n", str[0], stop, getpid());

	int single = str[0] == stop;

	do {
		printf("%s\n", str);

		md5((uint8_t*) str, len, hash);
		if(memcmp(search, hash, 16) == 0) {
			return 1;
		}

		int pos = len - 1;
		do {
			str[pos] = table[str[pos]];
			if(str[pos] == '0') {
				pos--;
			} else {
				break;
			}

			if(pos < 0) {
				return 0;
			}
		} while(pos >= 0);

	} while(single || str[0] != stop);

	return 0;
}

void parseHash(const char* str, const uint8_t *search) {
    for (int i = 0; i < 16; i++) {
		sscanf(str + 2*i, "%2x", &search[i]);
	}
}

