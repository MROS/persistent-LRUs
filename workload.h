#pragma once

#include "block.h"
#include <utility>
#include <vector>
#include <memory>

class Workload {
public:
	virtual shared_ptr<Block> next() = 0;
};

class HardCodedWorkload: public Workload {
public:
	vector<Block> blocks;
	int count = 0;
	explicit HardCodedWorkload(vector<Block> blocks): blocks(std::move(blocks)) { }

	shared_ptr<Block> next() override {
		if (count < blocks.size()) {
			return make_shared<Block>(blocks[count++]);
		}
		return nullptr;
	}
};

HardCodedWorkload simple_workload();
