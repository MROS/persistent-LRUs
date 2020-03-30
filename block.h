#pragma once
#include <iostream>
#include <cstdint>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include "transaction.h"

struct Block {
	int64_t height;
	std::vector<Transaction> txs;

	template <class Archive>
	void serialize( Archive & ar )
	{
		ar( height, txs );
	}
	// for debug
	void show() {
		std::cout << "區塊高度 = " << height << std::endl;
		for (auto tx: txs) {
			tx.show();
		}
	}
};
