#include "../LRU.h"
#include "mutable-lru.hpp"

template<typename Key, typename Value>
class SimpleCopyLRU : public LRU<Key, Value> {
public:
	LRUCache<Key, Value> cache;

	typedef LRU<Key, Value> Parent;
	typedef std::variant<Get<Key>, Put<Key, Value>> Cmd;

	explicit SimpleCopyLRU(size_t capacity) {
		this->cache = LRUCache<Key, Value>(capacity);
	}
	explicit SimpleCopyLRU(LRUCache<Key, Value> cache) {
		this->cache = cache;
	}
	std::shared_ptr<Parent> create(size_t capacity) {
		return std::make_shared<SimpleCopyLRU>(SimpleCopyLRU(capacity));
	}
	SimpleCopyLRU() = default;
	std::optional<Value> read_only_get(Key &key) {
		return cache.read_only_get(key);
	}
	std::shared_ptr<Parent> batch_operate(std::vector<Cmd> &cmds) {
		LRUCache new_cache = cache.copy();
		for (auto &cmd : cmds) {
			if (std::get_if<Get<Key>>(&cmd) != nullptr) {
				Get<Key> get = std::get<Get<Key>>(cmd);
				std::cerr << "get " << get.key << std::endl;
				new_cache.get(get.key);
			} else {
				Put<Key, Value> put = std::get<Put<Key, Value>>(cmd);
				std::cerr << "put " << put.key << " " << put.value << std::endl;
				new_cache.put(put.key, put.value);
			}
		}
		return std::make_shared<SimpleCopyLRU<Key, Value>>(SimpleCopyLRU<Key, Value>(new_cache));
	}
};
