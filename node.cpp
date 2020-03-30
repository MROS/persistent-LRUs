#include "node.h"
#include "api.h"
#include "constant.h"
#include <iostream>
#include <boost/asio.hpp>
#include <cstdlib>
#include <thread>
#include <cpptoml.h>
#include <sstream>

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
	bool is_eof = false;
	while (true) {
		Api api;
		try {
			is_eof = api.read(socket);
		} catch (exception& e) {
			cerr << e.what() << endl;
			break;
		}

		if (!is_eof) {
			cout << remote_endpoint.address() << ":" << remote_endpoint.port() << "結束" << endl;
			break;
		}

		switch (api.header.type) {
			case Type::TX: {
				cout << "收到交易，長度 = " << api.header.length << endl;
				stringstream ss(api.data);
				{
					cereal::BinaryInputArchive archive( ss );
					Transaction tx;
					archive(tx);
					tx.show();
				}
				break;
			}
			case Type::BLOCK: {
				cout << "收到區塊，長度 = " << api.header.length << endl;
				stringstream ss(api.data);
				{
					cereal::BinaryInputArchive archive( ss );
					Block block;
					archive(block);
					block.show();
				}

				break;
			}
		}
	}
}
