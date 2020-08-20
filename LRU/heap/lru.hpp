#include "../LRU.h"
#include "heap.hpp"
#include <immer/map.hpp>
#include <memory>
using namespace std;

template<typename Value>
struct Info {
	int timestamp;
	Value value;
};

template<typename Key, typename Value>
class HeapLRU : public LRU<Key, Value> {
public:
	typedef LRU<Key, Value> Parent;
	typedef std::variant<Get<Key>, Put<Key, Value>> Cmd;
	typedef HeapNode<Key> _Node;
	typedef Info<Value> _Info;

	int capacity;

	immer::map<Key, _Info> map;
	shared_ptr<Heap<Key>> heap;

	int timestamp;

	void maintain_root() {
		while (heap->root->timestamp != map[heap->root->key].timestamp) {
			heap->root->timestamp = map[heap->root->key].timestamp;
			heap->shiftdown(heap->root);
		}
	}

	explicit HeapLRU() = default;
	explicit HeapLRU(size_t capacity) {
		capacity = capacity;
		heap = make_shared<Heap<Key>>(capacity);
		timestamp = 0;
	}
	std::shared_ptr<Parent> create(size_t capacity) {
		return make_shared<HeapLRU>(capacity);
	}
	std::optional<Value> read_only_get(Key &key) {
		if (map.count(key)) {
			return map[key].value;
		} else {
			return nullopt;
		}
	}
	std::shared_ptr<Parent> batch_operate(std::vector<Cmd> &cmds) {
		auto new_cache = make_shared<HeapLRU<Key, Value>>(capacity);
		new_cache->heap = heap->new_version();
		new_cache->map = map;
		auto new_timestamp = timestamp;

		for (auto &cmd: cmds) {
			if (std::get_if<Get<Key>>(&cmd) != nullptr) {
				Get<Key> get = std::get<Get<Key>>(cmd);
//				printf("get (%d)\n", get.key);
				bool key_exist = new_cache->map.count(get.key);
				if (!key_exist) {
					// 快取 miss
					continue;
				} else {
					// 快取命中
					_Info info = new_cache->map[get.key];
					info.timestamp = new_timestamp++;
					new_cache->map = new_cache->map.set(get.key, info);
					new_cache->maintain_root();
				}
			} else {
				Put<Key, Value> put = std::get<Put<Key, Value>>(cmd);
//				printf("put (%d, %d)\n", put.key, put.value);
				bool key_exist = new_cache->map.count(put.key);
				if (!key_exist) {
					// 快取 miss
					if (new_cache->heap->is_full()) {
						// heap 已滿
						new_cache->map = new_cache->map.erase(new_cache->heap->root->key);
						new_cache->map = new_cache->map.set(put.key, { new_timestamp, put.value });
						new_cache->heap->root->timestamp = new_timestamp++;
						new_cache->heap->root->key = put.key;
						new_cache->heap->shiftdown(new_cache->heap->root);
						new_cache->maintain_root();
					} else {
						// heap 未滿
						new_cache->heap->add(put.key, new_timestamp);
						new_cache->map = new_cache->map.set(put.key, { new_timestamp++, put.value });
					}
				} else {
					// 快取命中
					new_cache->map = new_cache->map.set(put.key, { new_timestamp++, put.value });
					new_cache->maintain_root();
				}
			}
//			new_cache->heap->show();
		}
		new_cache->timestamp = new_timestamp;
		return new_cache;
	}
};
