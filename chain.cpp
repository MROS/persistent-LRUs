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

}

void Chain::add_block(Block &block) {
	auto res = blocks.find(block.prev_block_hash);
	if (res == blocks.end()) {
		cerr << "插入區塊時，找不到前一個區塊" << endl;
		return;
	}
	auto prev_block = res->second;

	// 驗證 edrax 證明
	for (const auto& tx: block.txs_with_proof) {
		if (!vc.verify(prev_block->digest, tx.from, mpz_class(tx.from_balance), tx.proof)) {
			cerr << "edrax 驗證錯誤" << endl;
			return;
		}
	}
	cout << "edrax 驗證成功" << endl;

	auto block_info = BlockInfo(block);

	blocks[block_info.hash] = make_shared<BlockInfo>(block_info);

	vector<hash_t> too_old;
	for (const auto& kv: blocks) {
		auto b = kv.second;
		if (b->block.height <= block.height - RESERVE_BLOCK_NUMBER) {
			too_old.push_back(b->hash);
		}
	}
	for (auto h: too_old) {
		blocks.erase(h);
	}

}

