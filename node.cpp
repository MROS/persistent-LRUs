#include "node.h"
#include "api.h"
#include "chain.h"
#include "channel.h"
#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include "config.h"

using namespace std;
using namespace boost::asio::ip;
using namespace boost::asio;
using namespace boost::system;

extern Config config;

Node::Node() {
	channel = make_shared<Channel<shared_ptr<Api>>>(Channel<shared_ptr<Api>>());
	this->port = config.port;
}

void Node::start()
{
	cout << "淺狀態區塊鏈，啓動！\n";

	// 開啓 tx, block 處理線程
	Chain chain(channel);
	thread c(&Chain::start, &chain);
	c.detach();

	// 開啓 socket 監聽線程
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
	bool is_eof = false;
	while (true) {
		auto api = make_shared<Api>();
		try {
			is_eof = api->read(socket);
		} catch (exception& e) {
			cerr << "handle_client 讀取錯誤" << endl;
			cerr << e.what() << endl;
			break;
		}

		if (!is_eof) {
			cout << remote_endpoint.address() << ":" << remote_endpoint.port() << "結束" << endl;
			break;
		}

		channel->add(api);

	}
}
