#pragma once
#include <iostream>
#include <cstdint>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include "transaction.h"

struct Block {
	uint64_t prev_block_hash;
	uint64_t height;
	uint64_t nonce;
	std::vector<Transaction> txs;

	template <class Archive>
	void serialize( Archive & ar )
	{
		ar( prev_block_hash, height, nonce, txs );
	}
	// for debug
	void show() {
		std::cout << "前一個區塊 = " << prev_block_hash << std::endl;
		std::cout << "區塊高度 = " << height << std::endl;
		for (auto tx: txs) {
			tx.show();
		}
	}
};
