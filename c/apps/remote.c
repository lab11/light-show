#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "main.h"
#include "app.h"
#include "effects.h"
#include "lights.h"

#define REMOTE_LIMIT_IN_SECONDS 60

static int server_socket;

void remote_control (uint32_t* lights, int len) {
	int client;

	client = accept(server_socket, NULL, NULL);
	if (client < 0) {
		perror("\naccept");
		return;
	}

	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	if (setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) != 0) {
		perror("\nsetsockopt client TO");
		goto remote_die;
	}

	uint32_t len32 = htonl(len);
	if (send(client, &len32, sizeof(uint32_t), MSG_NOSIGNAL) != sizeof(uint32_t)) {
		perror("\nsend len");
		goto remote_die;
	}

	                  // 1234567890
	const char *magic = "safety off";
	const int magic_len = 10;
	char buf[16];

	if (recv(client, buf, magic_len, MSG_WAITALL) != magic_len) {
		perror("\nrecv magic");
		goto remote_die;
	}

	if (0 != memcmp(buf, magic, magic_len)) {
		fprintf(stderr, "\nBad magic string\n");
		goto remote_die;
	}

	time_t start_time = time(NULL);
	const int net_len = len * sizeof(uint32_t);
	while (time(NULL) - start_time < REMOTE_LIMIT_IN_SECONDS) {
		if (recv(client, lights, net_len, MSG_WAITALL) != net_len) {
			perror("\nrecv lights");
			goto remote_die;
		}

		lights_set(lights, len);
	}

remote_die:
	close(client);
	return;
}


static int remote_init () {
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		perror("\nsocket");
		return -1;
	}

	struct sockaddr_in server = {0};
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(4908);

	int on = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0){
		perror("\nsetsockopt");
		return -1;
	}

	if (bind(server_socket, (struct sockaddr *) &server, sizeof server) != 0) {
		perror("\nbind");
		return -1;
	}

	if (listen(server_socket, 3) != 0) {
		perror("\nlisten");
		return -1;
	}

	int flags = fcntl(server_socket, F_GETFL, 0);
	if (flags == -1) {
		perror("\nfcntl");
		return -1;
	}
	flags |= O_NONBLOCK;
	if (fcntl(server_socket, F_SETFL, flags) != 0) {
		perror("\nfcntl");
		return -1;
	}

	register_socket(server_socket, remote_control);

	return 0;
}

__attribute__ ((constructor))
static void register_remote_init(void) {
	register_init_fn(remote_init, "remote");
}
