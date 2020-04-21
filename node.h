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
	Node();
	void handle_block();
	void handle_client(boost::asio::ip::tcp::socket socket);
    void start();
};