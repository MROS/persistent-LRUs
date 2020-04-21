#pragma once
#include "block.h"
#include "channel.h"
#include "edrax/vcs.h"
#include "VC.h"
#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <set>
#include <unordered_map>

using hash_t = uint64_t;

struct BlockInfo {
	Block block;
	hash_t hash;
	Ec1 digest;

	explicit BlockInfo(Block block) {
		this->block = block;
		this->hash = std::hash<Block *>{}(&this->block);
	}

};

class Chain {
	std::unordered_map<hash_t, std::shared_ptr<BlockInfo>> blocks;

	VC vc;

public:
	explicit Chain() {

		// 創世區塊
		// TODO: 思考是否能夠如此隨意設定
		auto genesis_block_hash = 0;
		auto block_info = std::make_shared<BlockInfo>(BlockInfo(Block()));
		block_info->digest = vc.a->g1 * 0;
		blocks[genesis_block_hash] = block_info;
	}

	void start();

	void add_block(Block &block);
//	void add_tx(Transaction &tx);
//	void add_tx_with_proof(TransactionWithProof &tx);

//	void broadcast(Block &block);
};
