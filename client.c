#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <netdb.h>
#include "crack.h"

#define BUFSIZE 1024
const char letters[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

int sendHash(const char *hostname, int port, char *hash, int size, char from, char to) {
	int sockfd, n;
	struct sockaddr_in serveraddr;
	struct hostent *server;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(1);
	}

	server = gethostbyname(hostname);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host as %s\n", hostname);
		exit(0);
	}

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
			(char *)&serveraddr.sin_addr.s_addr, server->h_length);
	serveraddr.sin_port = htons(port);

	if (connect(sockfd, (sockaddr*) &serveraddr, sizeof(serveraddr)) < 0) {
		printf("%s:%d\n", hostname, port);
		perror("ERROR connecting");
		exit(1);
	}

	printf("sending job to %s\n", hostname);
	char msg[1024];
	snprintf(msg, sizeof(msg), "%s %d %c %c", hash, size, from, to);
	write(sockfd, msg, strlen(msg));

	return sockfd;
}


int main(int argc, char **argv) {
	const char* servers[] = {
		"localhost",
		"158.196.22.154",
		"158.196.22.155",
		"158.196.22.156"
	};
	const int numServers = sizeof(servers) / sizeof(servers[0]);

	if(argc < 3) {
		printf("Usage: %s hash passsize\n", argv[0]);
		exit(1);
	}

	char *hash = argv[1];
	int len = atoi(argv[2]);

	int threads = 16;
	int fds[threads];
	for(int i = 0; i < threads; i++) {
		char from = letters[sizeof(letters) / threads * i];
		char to = letters[sizeof(letters) / threads * (i + 1)];

		fds[i] = sendHash(servers[i % numServers], 1234, hash, len, from, to);

	}

	fd_set set;
	FD_ZERO(&set);
	for(int i = 0; i < threads; i++) {
		FD_SET(fds[i], &set);
	}

	for(;;) {
		int ret = select(fds[threads - 1] + 1, &set, NULL, NULL, NULL);
		if(ret == -1) {
			perror("select");
			exit(1);
		}

		if(ret) {
			for(int i = 0; i < threads; i++) {
				if(FD_ISSET(fds[i], &set)) {
					char buffer[128];
					int size = read(fds[i], buffer, sizeof(buffer));
					buffer[size] = 0;
					printf("Found pass: %s\n", buffer);
					exit(1);
				}
			}
		}
	}

	return 0;
}
