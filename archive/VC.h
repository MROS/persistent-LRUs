#pragma once

#include <cstddef>
#include <utility>
#include <gmp.h>
#include <gmpxx.h>
#include "constant.h"
#include "test_point.hpp"
#include "edrax/vcs.h"
#include "bn.h"

// vector commitment
class VC {
public:
	vector<vector<Ec1> > prk; // proof key
	vector<Ec2> vrk;          // verify key

	// 只有 [0 ~ update_number) 的賬戶可能會被修改
	size_t update_key_number = EDRAX_UPDATE_KEY_NUMBER;
	vector<vector<Ec1>> upk;  // update key

	size_t bits = EDRAX_BITS;

	// TODO: 將 a 改名爲 edrax
	// a 是什麼腦殘命名？
	vcs *a;

	// vector 長爲 2 ^ bits
	VC(bool x) {
		if (x == false) { return; }
		printf("初始化 vector commitment, update key number = %lu, bits = %lu\n", update_key_number, bits);
		bn::CurveParam cp = bn::CurveFp254BNb;
		Param::init(cp);
		const Point& pt = selectPoint(cp);
		const Ec2 g2(
		Fp2(Fp(pt.g2.aa), Fp(pt.g2.ab)),
				Fp2(Fp(pt.g2.ba), Fp(pt.g2.bb))
		);
		const Ec1 g1(pt.g1.a, pt.g1.b);

		mpz_class p;
		p.set_str("16798108731015832284940804142231733909759579603404752749028378864165570215949",10);

		// 亂數種子永遠相同
		srand(0);
		gmp_randstate_t r_state;
		unsigned long int seed = rand();
		gmp_randinit_default (r_state);
		gmp_randseed_ui(r_state, seed);

		a = new vcs(bits, p, g1, g2);

		a->keygen(prk, vrk);
		a->load_key(prk,vrk);

		vector<long long> update_indexs;
		for(long long i = 0; i < update_key_number; i++) {
			update_indexs.push_back(i);
		}
		upk = a->calc_update_key_batch(update_indexs, prk);
	}
	// update_index 處發生修改
	// 欲取得 index 處的新證明
	vector<Ec1> update_proof(vector<Ec1> proof, int index, int update_index, mpz_class delta) {
		return a->update_proof(std::move(proof), update_index, index, std::move(delta), upk[update_index]);
	}
	Ec1 update_digest(Ec1 digest, int index, mpz_class delta) {
		return a->update_digest(digest, index, std::move(delta), upk[index]);
	}
	bool verify(Ec1 digest, int index, mpz_class a_i, vector<Ec1> proof) {
		return a->verify(digest, index, a_i, proof, vrk);
	}
};

VC get_vc();
