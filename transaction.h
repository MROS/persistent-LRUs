#pragma once
#include <iostream>
#include <cstdint>
#include "cereal/archives/binary.hpp"

struct Transaction {
	// nonce 代表付款人是第幾次付款
	// 以此來防止一個交易被多次使用
	int64_t from, to, value, nonce;
	template <class Archive>
	void serialize( Archive & ar )
	{
		ar( from, to, value, nonce );
	}

	// for debug
	void show() {
		std::cout << "from: " << from << ", to: " << to << ", value: " << value << ", nonce: " << nonce << std::endl;
	}
};
