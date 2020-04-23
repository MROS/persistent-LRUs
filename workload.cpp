#include "workload.h"
#include "constant.h"
#include "VC.h"

using namespace std;

HardCodedWorkload simple_workload() {
	vector<Block> blocks;
	Block block1 { 0, 1, 0, {}, {}};

	for (int64_t i = 1; i <= 20; i++) {
		TransactionWithProof tx;
		tx.from = 1;
		tx.to = i;
		tx.value = 100;
		tx.nonce = 0;
		tx.block_hash = 0;
		tx.from_balance = 0;
		tx.proof = vector<Ec1>(EDRAX_BITS, get_vc().a->g1*0);
		block1.txs_with_proof.push_back(tx);
	}
	blocks.push_back(block1);

	return HardCodedWorkload(blocks);

	//	Block block2 { BlockInfo(block1).hash, 2, 0, {}, {} };
	//
	//	for (int64_t i = 1; i <= 20; i++) {
	//		TransactionWithProof tx;
	//		tx.from = 1;
	//		tx.to = i;
	//		tx.value = 100;
	//		tx.nonce = 0;
	//		tx.block_hash = 0;
	//		tx.proof = vector<Ec1>(EDRAX_BITS, vc.a->g1*0);
	//		block1.txs_with_proof.push_back(tx);
	//	}

}
