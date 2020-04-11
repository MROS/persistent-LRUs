#pragma once
#include "block.h"
#include "channel.h"
#include "api.h"
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

	// 後繼區塊的 hash
	// std::vector< hash_t > successors;

	explicit BlockInfo(Block block) {
		this->block = block;
		this->hash = std::hash<Block *>{}(&this->block);
	}

};

enum class TXState {
	VALID,
	INVALID,
};

class Chain {
	std::shared_ptr<Channel<std::shared_ptr<Api>>> channel;
	std::unordered_map<hash_t, std::shared_ptr<BlockInfo>> blocks;

	// 依附於某個區塊的帶證明交易
	std::unordered_map<hash_t, std::unordered_set<std::shared_ptr<TransactionWithProof>>> txs_with_proof;

	// 不附證明的交易，意圖仰賴快取
	std::unordered_set<std::shared_ptr<Transaction>> txs;

	Block mining_block;

	hash_t cur_block_hash;

public:
	explicit Chain(std::shared_ptr<Channel<std::shared_ptr<Api>>> channel) {
		this->channel = std::move(channel);

		// 創世區塊
		// TODO: 思考是否能夠如此隨意設定
		cur_block_hash = 0;
		blocks[cur_block_hash] = std::make_shared<BlockInfo>(BlockInfo(Block()));
	}

	void start();

	void add_block(Block &block);
	void add_tx(Transaction &tx);
	void add_tx_with_proof(TransactionWithProof &tx);

	void broadcast(Block &block);

	// cur_block_hash 改變之後，重新生成一個正在產生的區塊 (mining_block)
	virtual void regen_block();
};
