#include "order_tree.hpp"
#include "../LRU.h"
#include <immer/map.hpp>
#include <optional>
#include <vector>
#include <memory>

using namespace std;

template<typename Key, typename Value>
class OrderTreeLRU : public LRU<Key, Value> {
private:
    size_t capacity{};
    size_t used{};
    immer::map<Key, shared_ptr<OrderTreeNode<Key, Value>>> map;
	shared_ptr<OrderTree<Key, Value>> order_tree{};

public:
	typedef LRU<Key, Value> Parent;
	typedef variant<Get<Key>, Put<Key, Value>> Cmd;

    explicit OrderTreeLRU(size_t capacity) {
		this->capacity = capacity;
		this->used = 0;
		int h = 0, c = capacity;
		while (c > 0) { c /= 2; h++; }
		this->order_tree = make_shared<OrderTree<Key, Value>>(h + 1);
		this->map = immer::map<Key, shared_ptr<OrderTreeNode<Key, Value>>>{};
    }
	OrderTreeLRU() = default;
	shared_ptr<Parent> create(size_t capacity) {
		return make_shared<OrderTreeLRU>(capacity);
	}
    shared_ptr<OrderTreeLRU> put(int key, int value) {
    	return nullptr;
	}
	optional<Value> read_only_get(Key &key) {
		shared_ptr<OrderTreeNode<Key, Value>> node = map[key];
		if (node == nullptr) {
			return nullopt;
		} else {
			return node->value;
		}
	}
	shared_ptr<Parent> batch_operate(vector<Cmd> &cmds) {
		auto tree = order_tree;
		auto new_cache = shared_ptr<OrderTreeLRU<Key, Value>>();
		new_cache->capacity = capacity;
		new_cache->used = used;
		new_cache->map = map;
		for (auto &cmd: cmds) {
			if (std::get_if<Get<Key>>(&cmd) != nullptr) {
				Get<Key> get = std::get<Get<Key>>(cmd);
				shared_ptr<OrderTreeNode<Key, Value>> node = map[get.key];
				if (node == nullptr) {
					continue;
				} else {
					auto ret = tree->get(node);
					tree = ret.new_tree;
					map[get.key] = ret.new_node;
				}
			} else {
				Put<Key, Value> put = std::get<Put<Key, Value>>(cmd);
				shared_ptr<OrderTreeNode<Key, Value>> node = map[put.key];
			}
		}
		return shared_ptr<Parent>();
	}
};