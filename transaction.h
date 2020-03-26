#pragma once
#include <cstdint>
#include "cereal/archives/binary.hpp"

struct Transaction {
	int64_t from, to, value;
	template <class Archive>
	void serialize( Archive & ar )
	{
		ar( from, to, value );
	}
};
