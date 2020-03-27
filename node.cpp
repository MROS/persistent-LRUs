#include "node.h"
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <thread>
#include <cpptoml.h>

using namespace std;

Node::Node(string &config_file) {
	auto config = cpptoml::parse_file(config_file);

	auto cache_policy = config->get_qualified_as<string>("cache.policy");
	if (cache_policy) {
		cout << "cache policy = " << *cache_policy << endl;
	} else {
		cerr << "無快取策略" << endl;
		exit(1);
	}

	auto port = config->get_qualified_as<int>("network.port").value_or(0);
	if (port) {
		this->port = port;
		cout << "port = " << this->port << endl;
	} else {
		cerr << "未指定埠口" << endl;
		exit(1);
	}

}

void Node::start()
{
	cout << "淺狀態區塊鏈，啓動！\n";
	init_network();

	struct sockaddr_in cli_addr{};
	socklen_t cli_len = sizeof(cli_addr);

	while (true) {
		int new_sock_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_len);
		if (new_sock_fd < 0) {
			perror("ERROR on accept");
			exit(1);
		}
		cout << "accept " << new_sock_fd << endl;
		thread t(&Node::handle_client, this, new_sock_fd);
		t.detach();
	}
}

void Node::handle_client(int fd) {
	char buffer[256];

	bzero(buffer, sizeof(buffer));

	int n = read(fd, buffer, 255);

	if (n < 0) {
		perror("ERROR reading from socket");
		exit(1);
	}

	printf("Here is the message: %s\n",buffer);

	/* Write a response to the client */
	n = write(fd, "I got your message", 18);

	if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}
}


int Node::init_network() {

	char buffer[256];
	int n;
	this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (this->sock_fd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	/* Initialize socket structure */
	struct sockaddr_in serv_addr{};
	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(this->port);

	if (bind(this->sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}

	// 若有 20 個以上的連線在等待 accept，拒絕連線
	// 詳見 listen 的 manual
	listen(this->sock_fd, 20);

	return 0;
}
