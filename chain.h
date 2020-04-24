#pragma once

#include "block.h"
#include "channel.h"
#include "edrax/vcs.h"
#include "workload.h"
#include "VC.h"
#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <set>
#include <unordered_map>

using hash_t = uint64_t;

struct BlockInfo {
	Block block;
	hash_t hash;
	Ec1 digest;

	explicit BlockInfo(Block block, Ec1 prev_digest) {
		this->block = std::move(block);
		// NOTE: 先用 nonce 當做 hash ，副作用是每個 nonce 都得不同
		this->hash = this->block.nonce;
		// TODO: 生成 digest
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

class Chain {
	std::unordered_map<hash_t, std::shared_ptr<BlockInfo>> blocks;

public:
	explicit Chain();

	void work(Workload &workload);

	void add_block(Block &block);
//	void add_tx(Transaction &tx);
//	void add_tx_with_proof(TransactionWithProof &tx);

//	void broadcast(Block &block);
};
