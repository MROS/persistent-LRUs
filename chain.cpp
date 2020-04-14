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

	// TODO: 直接在此 add_block 以進行試驗
	// TODO: 最後還是要做序列化
	Block block1 { 0, 1, 0, {}, {}};

	for (int64_t i = 1; i <= 20; i++) {
		TransactionWithProof tx;
		tx.from = 1;
		tx.to = i;
		tx.value = 100;
		tx.nonce = 0;
		tx.block_hash = 0;
		tx.from_balance = 0;
		tx.proof = vector<Ec1>(EDRAX_BITS, vc.a->g1*0);
		block1.txs_with_proof.push_back(tx);
	}

	add_block(block1);

	Block block2 { BlockInfo(block1).hash, 2, 0, {}, {} };

	for (int64_t i = 1; i <= 20; i++) {
		TransactionWithProof tx;
		tx.from = 1;
		tx.to = i;
		tx.value = 100;
		tx.nonce = 0;
		tx.block_hash = 0;
		tx.proof = vector<Ec1>(EDRAX_BITS, vc.a->g1*0);
		block1.txs_with_proof.push_back(tx);
	}

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
	auto prev_block = res->second;

	// 驗證 edrax 證明
	for (auto tx: block.txs_with_proof) {
		if (!vc.verify(prev_block->digest, tx.from, mpz_class(tx.from_balance), tx.proof)) {
			cerr << "edrax 驗證錯誤" << endl;
			return;
		}
	}
	cout << "edrax 驗證成功" << endl;

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


