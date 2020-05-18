#pragma once

#include <memory>

using namespace std;

template<typename T>
static T* make_mut(shared_ptr<T> *rc) {
	if (rc->use_count() > 1) {
		// 注意：這是唯一會複製東西的地方！！
		auto inner = *(rc->get());
		*rc = make_shared<T>(inner);

	}
	return rc->get();
}

template<typename T>
shared_ptr<T> take(shared_ptr<T> *rc) {
	shared_ptr<T> ret = nullptr;
	ret.swap(*rc);
	return ret;
}

