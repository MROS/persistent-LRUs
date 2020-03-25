#include "node.h"
#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>

void Node::start()
{
	printf("淺狀態區塊鏈，啓動！\n");
	init_network();
}

int Node::init_network() {

	int sock_fd, newsock_fd, portno;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (sock_fd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	/* Initialize socket structure */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = 5001;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}

	listen(sock_fd, 5);
	socklen_t clilen = sizeof(cli_addr);

	newsock_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &clilen);

	if (newsock_fd < 0) {
		perror("ERROR on accept");
		exit(1);
	}

	bzero(buffer,256);
	n = read(newsock_fd, buffer, 255 );

	if (n < 0) {
		perror("ERROR reading from socket");
		exit(1);
	}

	printf("Here is the message: %s\n",buffer);

	/* Write a response to the client */
	n = write(newsock_fd, "I got your message", 18);

	if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}

	return 0;
}
