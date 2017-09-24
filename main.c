#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <stdint.h>
#include <stdlib.h>

#define FOUND 1 
#define NOT_FOUND 0

void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest);

char table[255];

int crack(char* str, int len, char stop, uint8_t* search) {
	uint8_t hash[16];
	//printf("%c %c %d\n", str[0], stop, getpid());
	do {
//		printf("%s\n", str);

		md5((uint8_t*) str, len, hash);
		if(memcmp(search, hash, 16) == 0) {
//			printf("Found: %s\n", str);
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
		} while(pos >= 0);

	} while(str[0] != stop);

	return 0;
}

const char letters[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

int main(int argc, char **argv) {
	if(argc != 3) {
		printf("Usage: %s hash passsize\n", argv[0]);
		exit(1);
	}

	uint8_t search[16];	
	for (int i = 0; i < 16; i++) {
		sscanf(argv[1] + 2*i, "%2x", &search[i]);
	}

	int len = atoi(argv[2]);

	printf("Going to crack %d length...\n", len);
	for(int i = 0; i < sizeof(table) - 1; i++) {
		table[i] = i + 1;
	}
	table['9'] = 'A';
	table['Z'] = 'a';
	table['z'] = '0';


	char str[len + 1];
	memset(str, '0', len);
	str[len] = 0;

	int pipes[2];
	pipe(pipes);

	int threads = 4;
	pid_t running[threads];
	for(int i = 0; i < threads; i++) {
		running[i] = fork();
		if(running[i] == 0) {	
			close(pipes[0]);
			str[0] = letters[sizeof(letters) / threads * i];
			char last = letters[sizeof(letters) / threads * (i + 1)];
			if(i == threads - 1) {
				last = '0';
			}

			if(crack(str, len, last, search)) {
				write(pipes[1], str, len);
				return 1;
			}
			return 0;
		}
	}
	close(pipes[1]);

	int status;
	pid_t pid;
	while((pid = wait(&status)) > 0) {
		if(WEXITSTATUS(status) == FOUND) {
			printf("Password found:\n");
			for(int i = 0; i < threads; i++) {
				kill(running[i], SIGKILL);
			}

			char buf[32];
			int s;
			while(s = read(pipes[0], buf, sizeof(buf))) {
				buf[s] = 0;
				printf("%s", buf);
			}
			printf("\n");
		}

//		printf("%d end value: %d\n", pid, WEXITSTATUS(status));

	}


	return 0;
}
