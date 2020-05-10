#include "rb_tree.hpp"
#include "../LRU.h"
#include <vector>
#include <optional>

template<typename Key, typename Value>
class RBTreeLRU : public LRU<Key, Value> {
private:
    int capacity{};
	int counter = 0;
	RedBlackTree<Key, Value> rb_tree{};
public:
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
	std::shared_ptr<LRU<Key, Value>> batch_operate(std::vector<Cmd> &cmds) {
		auto next = make_shared<RBTreeLRU<Key, Value>>(this->capacity);
		next->counter = this->counter;
		next->rb_tree = this->rb_tree.clone_and_freeze();

		for (auto &&cmd : cmds) {
			auto get = get_if<Get<Key>>(&cmd);
			if (get != nullptr) {
				Key &key = get->key;
				auto node = next->rb_tree.find_by_id(key);
				if (node == nullptr) {
					// QUESTION: 嘗試讀取不在快取的東西，要報錯嗎？
				} else {
					auto value = node->get()->entry->value.value;
					next->rb_tree.remove(key);
					// TODO: batch?
					next->rb_tree.insert(next->counter++, key, value);
				}
			} else {
				auto put = get_if<Put<Key, Value>>(&cmd);
				Key &key = put->key;
				Value &value = put->value;
				auto node = next->rb_tree.find_by_id(key);
				if (node != nullptr) { // 在快取裡面
					next->rb_tree.remove(key);
				}
				// TODO: batch?
				next->rb_tree.insert(next->counter++, key, value);
			}
		}
		while (next->rb_tree.size() > next->capacity) {
			next->rb_tree.remove_least();
		}
		return next;
	}
};