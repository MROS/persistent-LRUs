#include "chain.h"
#include "constant.h"
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
					add_block(block);
				}
				break;
			}
		}
	}
}

void Chain::add_block(Block block) {
	auto res = blocks.find(block.prev_block_hash);
	if (res == blocks.end()) {
		cerr << "插入區塊時，找不到前一個區塊" << endl;
		return;
	}

	auto cur_block = BlockInfo(block);

	blocks[cur_block.hash] = make_shared<BlockInfo>(cur_block);

	vector<hash_t> too_old;
	for (auto kv: blocks) {
		auto b = kv.second;
		if (b->block.height <= block.height - RESERVE_BLOCK_NUMBER) {
			too_old.push_back(b->hash);
		}
	}
	for (auto h: too_old) {
		blocks.erase(h);
	}

	// TODO: 同時要清除 tx_pool

	cur_block_hash = cur_block.hash;
	regen_block();
}

void Chain::add_tx(Transaction tx) {
	tx_pool.insert(tx);
}

