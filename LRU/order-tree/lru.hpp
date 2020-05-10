#include "order_tree.hpp"
#include "../LRU.h"
#include <immer/map.hpp>
#include <optional>
#include <vector>
#include <memory>

template<typename Key, typename Value>
class OrderTreeLRU : public LRU<Key, Value> {
private:
    size_t capacity{};
    size_t used{};
    immer::map<Key, Node<Value> *> map;
	OrderTree<Key, Value> *order_tree{};

public:
	typedef LRU<Key, Value> Parent;
	typedef std::variant<Get<Key>, Put<Key, Value>> Cmd;

    explicit OrderTreeLRU(size_t capacity) {
		this->capacity = capacity;
		this->used = 0;
		int h = 0, c = capacity;
		while (c > 0) { c /= 2; h++; }
		this->order_tree = new OrderTree<Key, Value>(h + 1);
		this->map = immer::map<Key, Node<Value>*>{};
    }
	OrderTreeLRU() = default;
	std::pair<OrderTreeLRU*, std::optional<Value>> get(int key) {
		Node<Value> *node = map[key];
		if (node == nullptr) {
			return make_pair(this, std::nullopt);
		}
		auto get_ret = order_tree->get(node);
		auto new_cache = new OrderTreeLRU<Key, Value>();
		new_cache->capacity = capacity;
		new_cache->used = used;
		new_cache->order_tree = get_ret.new_tree;
		new_cache->map = this->map.set(key, get_ret.new_node);

		return make_pair(new_cache, node->value);
	}
    OrderTreeLRU* put(int key, int value) {
    	return nullptr;
	}
	std::optional<Value> read_only_get(Key &key) {
		Node<Value> *node = map[key];
		if (node == nullptr) {
			return std::nullopt;
		} else {
			return node->value;
		}
	}
	std::shared_ptr<Parent> batch_operate(std::vector<Cmd> &cmds) {
		return std::shared_ptr<Parent>();
	}
};