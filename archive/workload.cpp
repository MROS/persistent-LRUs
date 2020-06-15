#include "workload.h"
#include "constant.h"
#include "chain.h"
#include "VC.h"
#include <unordered_map>
#include <utility>

using namespace std;

Proof update_block(Proof proof, int index, const Block& block) {
	Proof new_proof = std::move(proof);
	for (auto tx: block.txs) {
		new_proof = get_vc().update_proof(new_proof, index, tx.from, mpz_class(-tx.value));
		new_proof = get_vc().update_proof(new_proof, index, tx.to, mpz_class(tx.value));
	}
	for (const auto& tx: block.txs_with_proof) {
		new_proof = get_vc().update_proof(new_proof, index, tx.from, mpz_class(-tx.value));
		new_proof = get_vc().update_proof(new_proof, index, tx.to, mpz_class(tx.value));
	}
	return new_proof;
}

HardCodedWorkload simple_workload() {
	vector<Block> blocks;
	Block block1 { 0, 1, 9487, {}, {}};

	unordered_map<int64_t, Proof> proofs;

	for (int64_t i = 0; i <= 20; i++) {
		proofs[i] = vector<Ec1>(EDRAX_BITS, get_vc().a->g1 * 0);
	}

	for (int64_t i = 1; i <= 20; i++) {
		TransactionWithProof tx;
		tx.from = 0;
		tx.to = i;
		tx.value = 100;
		tx.nonce = 0;
		tx.from_balance = 0;
		tx.proof = proofs[tx.from];
		block1.txs_with_proof.push_back(tx);
	}

	for (int64_t i = 0; i <= 20; i++) {
		proofs[i] = update_block(proofs[i], i, block1);
	}

	BlockInfo block_info1(block1, get_vc().a->g1 * 0);
	blocks.push_back(block1);

	printf("block_info1.hash = %lu\n", block_info1.hash);

	Block block2 { block_info1.hash, 2, 99999, {}, {} };

	for (int64_t i = 1; i <= 20; i++) {
		TransactionWithProof tx;
		tx.from = 0;
		tx.to = i;
		tx.value = 100;
		tx.nonce = 0;
		tx.from_balance = -2000;
		tx.proof = proofs[tx.from];
		block2.txs_with_proof.push_back(tx);
	}
	blocks.push_back(block2);

	return HardCodedWorkload(blocks);

}
