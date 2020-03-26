#pragma once
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
};
