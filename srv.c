#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "crack.h"

const char letters[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

#define panic(msg) perror(msg); exit(1);

int main(int argc, char **argv) {
	const int port = 1234;

	struct sockaddr_in srvAddr;
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if(!s) {
		perror("socket");
		exit(1);
	}

	int reuse = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
		perror("setsockopt failed");
	}

	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(port);
	srvAddr.sin_addr.s_addr = INADDR_ANY;

	if(bind(s, (struct sockaddr*) &srvAddr, sizeof(srvAddr)) != 0) {
		perror("bind");
		exit(1);
	}

	listen(s, 10);

	initTable();

	for(;;) {
		struct sockaddr_in clientAddr;
		char buf[1024];
		int n;

		socklen_t socketSize = sizeof(clientAddr);
		int client = accept(s, (struct sockaddr *) &clientAddr, &socketSize);
		if(fork() == 0) {
			close(s);

			n = read(client, buf, sizeof(buf));
			buf[n] = 0;

			printf("received: %s\n", buf);

			// hash len from to
			char hash[64];
			char from, to;
			int len;
			sscanf(buf, "%s %d %c %c", hash, &len, &from, &to);

			printf("going %s %d %c %c\n", hash, len, from, to);

			uint8_t search[16];
			parseHash(hash, search);

			char str[len + 1];
			memset(str, '0', len);
			str[len] = 0;

			str[0] = from;
			if(crack(str, len, to, search)) {
				printf("found\n");
				write(client, str, strlen(str));
			}

			close(client);
			printf("end\n");
			exit(0);
		}
		close(client);
	}


	return 0;
}

