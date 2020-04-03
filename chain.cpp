#include "chain.h"
#include <iostream>
#include <memory>

using namespace std;

void Chain::start() {
	cout << "chain start" << endl;
	while (true) {
		auto api = channel->remove();
		switch (api->header.type) {
			case Type::TX: {
				cout << "######## 收到交易，長度 = " << api->header.length << endl;
				stringstream ss(api->data);
				{
					cereal::BinaryInputArchive archive( ss );
					auto tx = make_shared<Transaction>();
					archive(*tx);
					tx->show();
				}
				break;
			}
			case Type::BLOCK: {
				cout << "######## 收到區塊，長度 = " << api->header.length << endl;
				stringstream ss(api->data);
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
