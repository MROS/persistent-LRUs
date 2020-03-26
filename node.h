#pragma once

#include <string>

class Node
{
	int port;
	int sock_fd;
public:
	Node(std::string &config_file);
	int init_network();
	void handle_client(int fd);
    void start();
};