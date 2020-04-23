#pragma once

#include "block.h"
#include "channel.h"
#include "edrax/vcs.h"
#include "workload.h"
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
		this->block = std::move(block);
		this->hash = std::hash<Block *>{}(&this->block);
		// TODO: 生成 digest
	}

};

class Chain {
	std::unordered_map<hash_t, std::shared_ptr<BlockInfo>> blocks;

public:
	explicit Chain();

	void work(Workload &workload);

	void add_block(Block &block);
//	void add_tx(Transaction &tx);
//	void add_tx_with_proof(TransactionWithProof &tx);

//	void broadcast(Block &block);
};
