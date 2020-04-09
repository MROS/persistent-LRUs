#pragma once
#include "block.h"
#include "channel.h"
#include "api.h"
#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <unordered_map>

struct BlockInfo {
	Block block;
	uint64_t hash;

	// 後繼區塊的 hash
//	std::vector< uint64_t > successors;

	explicit BlockInfo(Block block) {
		this->block = block;
		this->hash = std::hash<Block *>{}(&this->block);
	}

};

class Chain {
	std::shared_ptr<Channel<std::shared_ptr<Api>>> channel;
	std::unordered_map<uint64_t, std::shared_ptr<BlockInfo>> blocks;
public:
	explicit Chain(std::shared_ptr<Channel<std::shared_ptr<Api>>> channel) {
		this->channel = std::move(channel);
	}
	void add_block(Block block);
	void start();
};
