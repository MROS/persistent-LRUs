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
	// 簡單點處理，太舊的直接丟掉
	std::set<Transaction> tx_pool;

	Block mining_block;

	hash_t cur_block_hash;
public:
	explicit Chain(std::shared_ptr<Channel<std::shared_ptr<Api>>> channel) {
		this->channel = std::move(channel);
	}
	void start();

	void add_block(Block block);
	void add_tx(Transaction tx);

	// cur_block_hash 改變之後，重新生成一個正在產生的區塊 (mining_block)
	virtual void regen_block();

	virtual bool is_valid_now(Transaction tx);
};
