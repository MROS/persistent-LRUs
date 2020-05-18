#include "order_tree.hpp"
#include "../LRU.h"
#include <immer/map.hpp>
#include <optional>
#include <vector>
#include <memory>
#include <cmath>

using namespace std;

template<typename Key, typename Value>
class OrderTreeLRU : public LRU<Key, Value> {
private:
	typedef LRU<Key, Value> Parent;
	typedef variant<Get<Key>, Put<Key, Value>> Cmd;
	typedef OrderTreeNode<Key, Value> _Node;

    size_t capacity{};
    size_t used{};
    immer::map<Key, shared_ptr<OrderTreeNode<Key, Value>>> map;
	shared_ptr<OrderTree<Key, Value>> order_tree{};

public:

    explicit OrderTreeLRU(size_t capacity) {
		this->capacity = capacity;
		this->used = 0;
		int h = log2(capacity) + 2;
//		printf("capacity = %lu, height = %d\n", capacity, h);
		this->order_tree = make_shared<OrderTree<Key, Value>>(h);
		this->map = immer::map<Key, _Node*>{};
    }
    explicit OrderTreeLRU(OrderTreeLRU *lru) {
    	this->capacity = lru->capacity;
    	this->used = lru->used;
    	this->order_tree = lru->order_tree;
    	this->map = lru->map;
    }
	OrderTreeLRU() = default;
	shared_ptr<Parent> create(size_t capacity) {
		return make_shared<OrderTreeLRU>(capacity);
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
		auto new_tree = order_tree->new_version();
		auto new_cache = make_shared<OrderTreeLRU<Key, Value>>(this);
		auto new_map = map;

		for (auto &cmd: cmds) {
			if (std::get_if<Get<Key>>(&cmd) != nullptr) {
				Get<Key> get = std::get<Get<Key>>(cmd);
				_Node* node = new_map[get.key];
				if (node == nullptr) {
					continue;
				} else {
					auto ret = new_tree->get(node);
					if (ret.reorder != nullptr) {
						new_map = {};
						for (auto node : *ret.reorder) {
							new_map = new_map.set(node->key, node);
						}
						for (auto iter = new_map.begin(); iter != new_map.end(); iter++) {
							auto node = iter->second;
						}
					} else {
						new_map = new_map.set(get.key, ret.new_node);
					}
				}
			} else {
				Put<Key, Value> put = std::get<Put<Key, Value>>(cmd);
				shared_ptr<OrderTreeNode<Key, Value>> node = new_map[put.key];
				if (node == nullptr && new_cache->used < new_cache->capacity) {
					new_cache->used++;
					auto ret = new_tree->add(put.key, put.value);
					new_tree = ret.first;
					new_map = new_map.set(put.key, ret.second);
				} else {
					auto put_ret = new_tree->put(node, put.key, put.value);
					new_tree = put_ret.new_tree;
					if (put_ret.deleted_node != nullptr) {
						new_map = new_map.erase(put_ret.deleted_node->key);
					}
					if (put_ret.reorder != nullptr) {
						new_map = {};
						for (auto node : *put_ret.reorder) {
							new_map = new_map.set(node->key, node);
						}
					} else {
						new_map = new_map.set(put.key, put_ret.new_node);
					}
				}
			}
		}
//		new_tree->show();
		new_cache->order_tree = new_tree;
		new_cache->map = new_map;
		return new_cache;
	}
};