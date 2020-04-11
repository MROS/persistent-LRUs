#include "node.h"
#include "api.h"
#include "chain.h"
#include "channel.h"
#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <cstdlib>
#include <thread>
#include <cpptoml.h>

using namespace std;
using namespace boost::asio::ip;
using namespace boost::asio;
using namespace boost::system;

// TODO: 使用一個全域變數來儲存 config ，並在 main.cpp 初始化
Node::Node(string &config_file) {
	channel = make_shared<Channel<shared_ptr<Api>>>(Channel<shared_ptr<Api>>());
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

void Node::handle_transaction() {

}
