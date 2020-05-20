#include "../LRU.h"
#include "mutable-lru.hpp"
#include <memory>
using namespace std;

template<typename Key, typename Value>
class SimpleCopyLRU : public LRU<Key, Value> {
public:
	shared_ptr<LRUCache<Key, Value>> cache;

	typedef LRU<Key, Value> Parent;
	typedef std::variant<Get<Key>, Put<Key, Value>> Cmd;

	explicit SimpleCopyLRU(size_t capacity) {
		this->cache = make_shared<LRUCache<Key, Value>>(capacity);
	}
	explicit SimpleCopyLRU(shared_ptr<LRUCache<Key, Value>> cache) {
		this->cache = cache;
	}
	SimpleCopyLRU() = default;
	std::shared_ptr<Parent> create(size_t capacity) {
		return std::make_shared<SimpleCopyLRU>(capacity);
	}
	std::optional<Value> read_only_get(Key &key) {
		return cache->read_only_get(key);
	}
	std::shared_ptr<Parent> batch_operate(std::vector<Cmd> &cmds) {
		shared_ptr<LRUCache<Key, Value>> new_cache = cache->copy();
		for (auto &cmd : cmds) {
			if (std::get_if<Get<Key>>(&cmd) != nullptr) {
				Get<Key> get = std::get<Get<Key>>(cmd);
				new_cache->get(get.key);
			} else {
				Put<Key, Value> put = std::get<Put<Key, Value>>(cmd);
				new_cache->put(put.key, put.value);
			}
		}
		return std::make_shared<SimpleCopyLRU<Key, Value>>(new_cache);
	}
};
