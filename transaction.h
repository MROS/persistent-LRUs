#pragma once
#include <iostream>
#include <cstdint>
#include "cereal/archives/binary.hpp"

using hash_t = uint64_t;

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
		std::cout << "交易：" << std::endl;
		std::cout << "from: " << from << ", to: " << to << ", value: " << value << ", nonce: " << nonce << std::endl;
	}
};

// TODO: 設計 proof 結構
struct Proof {
	hash_t todo;
};

struct TransactionWithProof {
	int64_t from, to, value, nonce;
	hash_t block_hash;
	Proof proof;
	template <class Archive>
	void serialize( Archive & ar )
	{
		ar( from, to, value, nonce, block_hash, proof );
	}

	// for debug
	void show() {
		std::cout << "附證明交易：" << std::endl;
		std::cout << "block_hash: " << from << ", to: " << to << std::endl;
		std::cout << "from: " << from << ", to: " << to << ", value: " << value << ", nonce: " << nonce << std::endl;
	}
};
