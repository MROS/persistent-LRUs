#include "rb_tree.hpp"
#include "../LRU.h"
#include <vector>
#include <optional>

template<typename Key, typename Value>
class RBTreeLRU : public LRU<Key, Value> {
private:
    int capacity{};
	RedBlackTree<Key, Value> rb_tree{};
public:
	typedef RBTreeLRU<Key, Value> Self;
	typedef std::variant<Get<Key>, Put<Key, Value>> Cmd;

    explicit RBTreeLRU(int capacity) {
        this->capacity = capacity;
    }
    RBTreeLRU() = default;

	std::optional<Value> read_only_get(Key &key) {
        auto node = this->rb_tree.find_by_id(key);
		if (node == nullptr) {
			return std::nullopt;
		} else {
			return node->get()->entry->value.value;
		}
	}
	std::shared_ptr<Self> batch_operate(std::vector<Cmd> &cmds) {
		return std::shared_ptr<Self>();
	}
};