#pragma once
#include "block.h"
#include "channel.h"
#include "api.h"
#include <functional>
#include <memory>
#include <utility>
#include <vector>

struct BlockInfo {
	Block block;
	uint64_t hash;
	std::vector< std::shared_ptr<Block> > next;

	explicit BlockInfo(Block &&block) {
		this->block = block;
		this->hash = std::hash<Block *>{}(&this->block);
	}

};

class Chain {
	std::shared_ptr<Channel<std::shared_ptr<Api>>> channel;
public:
	explicit Chain(std::shared_ptr<Channel<std::shared_ptr<Api>>> channel) {
		this->channel = std::move(channel);
	}
	void start();
};
