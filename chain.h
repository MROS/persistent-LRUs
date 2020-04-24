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

class Chain {
	std::unordered_map<hash_t, std::shared_ptr<BlockInfo>> blocks;

public:
	explicit Chain();

	void work(Workload &workload);

	virtual bool verify_block(Block &block, BlockInfo &prev_block_info);
	void add_block(Block &block);
//	void add_tx(Transaction &tx);
//	void add_tx_with_proof(TransactionWithProof &tx);
//	void broadcast(Block &block);
};

// 快取最近 K 塊
class ChainNearK: public Chain {
public:
	bool verify_block(Block &block, BlockInfo &prev_block_info) override;
};

// 快取最近 K 塊
class ChainLRU: public Chain {
public:
	bool verify_block(Block &block, BlockInfo &prev_block_info) override;
};
