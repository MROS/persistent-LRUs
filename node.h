#pragma once

#include <string>
#include <boost/asio.hpp>
#include <memory>
#include "channel.h"
#include "api.h"

class Node
{
	int port;
	std::shared_ptr<Channel<std::shared_ptr<Api>>> channel;
public:
	explicit Node(std::string &config_file);
	void handle_transaction();
	void handle_block();
	void handle_client(boost::asio::ip::tcp::socket socket);
    void start();
};