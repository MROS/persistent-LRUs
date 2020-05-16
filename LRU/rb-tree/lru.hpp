#include "rb_tree.hpp"
#include "../LRU.h"
#include <vector>
#include <optional>

template<typename Key, typename Value>
class RBTreeLRU : public LRU<Key, Value> {
	typedef LRU<Key, Value> Parent;
private:
    int capacity{};
	int counter = 0;
	RedBlackTree<Key, Value> rb_tree{};
public:
	typedef std::variant<Get<Key>, Put<Key, Value>> Cmd;

    explicit RBTreeLRU(size_t capacity) {
        this->capacity = capacity;
    }
    RBTreeLRU() = default;

	std::shared_ptr<Parent> create(size_t capacity) {
		return std::make_shared<RBTreeLRU>(RBTreeLRU(capacity));
	}

	std::optional<Value> read_only_get(Key &key) {
        auto node = this->rb_tree.find_by_id(key);
		if (node == nullptr) {
			return std::nullopt;
		} else {
			return node->get()->entry->value.value;
		}
	}
	std::shared_ptr<LRU<Key, Value>> batch_operate(std::vector<Cmd> &cmds) {
		auto next = make_shared<RBTreeLRU<Key, Value>>(this->capacity);
		next->counter = this->counter;
		next->rb_tree = this->rb_tree.clone_and_freeze();

		for (auto &&cmd : cmds) {
			auto get = get_if<Get<Key>>(&cmd);
			if (get != nullptr) {
				Key &id = get->key;
				auto node = next->rb_tree.find_by_id(id);
				if (node == nullptr) {
					// QUESTION: 嘗試讀取不在快取的東西，要報錯嗎？
				} else {
					auto value = node->get()->entry->value.value;
					next->rb_tree.remove(node->get()->entry->key, id);
					// TODO: batch?
					next->rb_tree.insert(next->counter++, id, value);
				}
			} else {
				auto put = get_if<Put<Key, Value>>(&cmd);
				Key &id = put->key;
				Value &value = put->value;
				auto key = next->rb_tree.find_key_by_id(id);
				if (key.has_value()) { // 在快取裡面
					next->rb_tree.remove(key.value(), id);
				}
				// TODO: batch?
				next->rb_tree.insert(next->counter++, id, value);
			}
		}
		while (next->rb_tree.size() > next->capacity) {
			next->rb_tree.remove_least();
		}
		return next;
	}
};