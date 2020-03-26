#pragma once

#include <string>

class Node
{
public:
	Node(std::string &config_file);
	int init_network();
    void start();
};