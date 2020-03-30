#pragma once

#include <string>
#include <boost/asio.hpp>
#include <memory>

class Node
{
	int port;
public:
	explicit Node(std::string &config_file);
	void handle_client(boost::asio::ip::tcp::socket socket);
    void start();
};