#include "chain.h"
#include "constant.h"
#include <iostream>
#include <memory>

#include <boost/asio.hpp>
using namespace boost::asio::ip;
using namespace boost::asio;
using namespace boost::system;

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
					Transaction tx;
					archive(tx);
					tx.show();
					add_tx(tx);
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

void Chain::add_block(Block &block) {
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

void Chain::add_tx(Transaction &tx) {
	txs.insert(make_shared<Transaction>(tx));
	// TODO: 考慮將 Block 的 txs 型別改爲 shared_ptr
	mining_block.txs.push_back(tx);
	if (mining_block.txs.size() >= TX_PER_BLOCK) {
		add_block(mining_block);
		broadcast(mining_block);
	}
}

void Chain::add_tx_with_proof(TransactionWithProof &tx) {
	// TODO: 處理 tx.block_hash 還不在 blocks 的狀況
	txs_with_proof[tx.block_hash].insert(make_shared<TransactionWithProof>(tx));
}

void Chain::broadcast(Block &block) {
	std::cout << "廣播區塊" << std::endl;
	io_service iosev;
	tcp::socket socket(iosev);
	// TODO: 從 config 中讀取
	tcp::endpoint ep(ip::address_v4::from_string("127.0.0.1"), 1810);
	boost::system::error_code ec;
	socket.connect(ep, ec);
	if (ec) {
		std::cerr << "嘗試連線到 peer ，失敗" << std::endl;
		std::cerr << boost::system::system_error(ec).what() << std::endl;
		return;
	}

	Api(block).write(socket);
}

// TODO: 實作（或讓子類來實作）
void Chain::regen_block() {
	mining_block = {};
}


