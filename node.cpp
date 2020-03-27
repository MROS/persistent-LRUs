#include "node.h"
#include <iostream>
#include <boost/asio.hpp>
#include <cstdlib>
#include <thread>
#include <cpptoml.h>

using namespace std;
using namespace boost::asio::ip;
using namespace boost::asio;
using namespace boost::system;

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
	io_service iosev;
	ip::tcp::acceptor acceptor(iosev,
							   tcp::endpoint(tcp::v4(), this->port));

	while (true) {
		tcp::socket socket(iosev);
		acceptor.accept(socket);
		thread t(&Node::handle_client, this, std::move(socket));
		t.detach();
	}
}

void Node::handle_client(tcp::socket socket) {
	auto remote_endpoint = socket.remote_endpoint();
	cout << "accept " << remote_endpoint.address() << ":" << remote_endpoint.port() << endl;
	while (true) {
		char buf[128];
		boost::system::error_code ec;
		size_t len = socket.read_some(buffer(buf, 100), ec);
		buf[len] = '\0';

		if (ec)
		{
			cout << boost::system::system_error(ec).what() << endl;
			break;
		}

		cout << buf << endl;

	}
	return;
}
