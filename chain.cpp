#include "chain.h"
#include "constant.h"
#include "VC.h"
#include <iostream>
#include <memory>
using namespace std;

Chain::Chain() {
	// 創世區塊
	auto genesis_block_hash = 0;
	// 創世區塊的 prev_digest 不重要
	auto block_info = std::make_shared<BlockInfo>(BlockInfo(Block(), get_vc().a->g1 * 0));
	// TODO: 初始要分配大家一些錢
	block_info->digest = get_vc().a->g1 * 0;
	blocks[genesis_block_hash] = block_info;
}

void Chain::add_block(Block &block) {
	auto res = blocks.find(block.prev_block_hash);
	if (res == blocks.end()) {
		cerr << "插入區塊時，找不到前一個區塊 " << block.prev_block_hash << endl;
		exit(1);
	}
	auto prev_block = res->second;

	if (!verify_block(block, *prev_block)) {
		cerr << "驗證區塊失敗" << endl;
		exit(1);
	}

	auto block_info = BlockInfo(block, blocks[block.prev_block_hash]->digest);
	blocks[block_info.hash] = make_shared<BlockInfo>(block_info);

	// XXX: 每次都遍歷會不會太慢
	vector<hash_t> too_old;
	for (const auto& kv: blocks) {
		shared_ptr<BlockInfo> b = kv.second;
		if (b->block.height <= block.height - RESERVE_BLOCK_NUMBER) {
			too_old.push_back(b->hash);
		}
	}
	for (auto h: too_old) {
		blocks.erase(h);
	}

}

void Chain::work(Workload &workload) {
	shared_ptr<Block> block;
	while ((block = workload.next()) != nullptr) {
		add_block(*block);
	}
}

bool Chain::verify_block(Block &block, BlockInfo &prev_block_info) {

	// 驗證 edrax 證明
	for (const auto& tx: block.txs_with_proof) {
		cout << "tx.from: " << tx.from << ", tx.from_balance: " << tx.from_balance << endl;
		if (!get_vc().verify(prev_block_info.digest, tx.from, mpz_class(tx.from_balance), tx.proof)) {
			cerr << "edrax 驗證錯誤" << endl;
			return false;
		}
	}
	cout << "edrax 驗證成功" << endl;
	return true;
}


