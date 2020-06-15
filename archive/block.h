#pragma once
#include <iostream>
#include <cstdint>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include "transaction.h"
#include "VC.h"

struct Block {
	uint64_t prev_block_hash;
	int64_t height;
	uint64_t nonce;
	std::vector<Transaction> txs;
	std::vector<TransactionWithProof> txs_with_proof;

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

using hash_t = uint64_t;

struct BlockInfo {
	Block block;
	hash_t hash;
	Ec1 digest;

	explicit BlockInfo(Block block, Ec1 prev_digest) {
		this->block = std::move(block);
		// NOTE: 先用 nonce 當做 hash ，副作用是每個 nonce 都得不同
		this->hash = this->block.nonce;

		Ec1 new_digest = prev_digest;
		for (auto tx: this->block.txs) {
			new_digest = get_vc().update_digest(new_digest, tx.from, mpz_class(-tx.value));
			new_digest = get_vc().update_digest(new_digest, tx.to, mpz_class(tx.value));
		}
		for (const auto& tx: this->block.txs_with_proof) {
			new_digest = get_vc().update_digest(new_digest, tx.from, mpz_class(-tx.value));
			new_digest = get_vc().update_digest(new_digest, tx.to, mpz_class(tx.value));
		}
		this->digest = new_digest;
	}
};
