#pragma once
#include <iostream>
#include <cstdint>
#include "cereal/archives/binary.hpp"

struct Transaction {
	int64_t from, to, value;
	template <class Archive>
	void serialize( Archive & ar )
	{
		ar( from, to, value );
	}

	// for debug
	void show() {
		std::cout << "from: " << from << ", to: " << to << ", value: " << value << std::endl;
	}
};
