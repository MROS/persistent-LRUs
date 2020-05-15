#include <iostream>
#include <unordered_map>
#include "../LRU.h"
#include <optional>
#include <variant>

template<typename Key, typename Value>
struct Node {
    Node *next, *last;
    Key key;
    Value value;
    Node() {
        next = nullptr;
        last = nullptr;
    }
    Node(Node *node) {
		this->key = node->key;
		this->value = node->value;
		next = nullptr;
		last = nullptr;
	}
};

template<typename Key, typename Value>
class LRUCache {
	using _Node = Node<Key, Value>;
private:
    size_t capacity{};
    int used{};
    std::unordered_map<Key, _Node*> map;
    _Node *lru_head; // 最新
    _Node *lru_tail; // 最舊
    void show() {
		std::cout << "forward: ";
		for (_Node *iter = lru_head; iter != nullptr; iter = iter->next) {
        	std::cout << "(" << iter->key << ", " << iter->value << ")";
        }
        std::cout << std::endl;
        std::cout << "backward: ";
        for (_Node *iter = lru_tail; iter != nullptr; iter = iter->last) {
			std::cout << "(" << iter->key << ", " << iter->value << ")";
        }
		std::cout << std::endl;
    }
    void to_head(_Node *node) {
        if (lru_head == lru_tail || lru_head == node) {
            // do nothing
        } else if (lru_tail == node) {
            lru_tail = node->last;
            
            node->last->next = nullptr;
            node->last = nullptr;
            node->next = lru_head;
            lru_head->last = node;
            
            lru_head = node;
        } else {
            node->last->next = node->next;
            node->next->last = node->last;
            
            node->last = nullptr;
            node->next = lru_head;
            lru_head->last = node;
            lru_head = node;
        }
    }
public:
    explicit LRUCache(size_t capacity) {
        this->capacity = capacity;
        this->used = 0;
        lru_head = nullptr;
        lru_tail = nullptr;
    }

    LRUCache() = default;

    LRUCache copy() {
    	LRUCache<Key, Value> new_cache(capacity);
		new_cache.used = used;
		new_cache.map = {};
		auto iter = lru_head;
		if (iter == nullptr) {
			new_cache.lru_head = nullptr;
			new_cache.lru_tail = nullptr;
		} else {
			auto new_iter = new _Node(iter);
			new_cache.map[new_iter->key] = new_iter;
			new_cache.lru_head = new_iter;
			while (iter != lru_tail) {
				iter = iter->next;
				new_iter->next = new _Node(iter);
				new_iter->next->last = new_iter;
				new_iter = new_iter->next;
				new_cache.map[new_iter->key] = new_iter;
			}
			new_cache.lru_tail = new_iter;
		}
    	return new_cache;
    }

	std::optional<Value> read_only_get(Key &key) {
    	if (map.count(key)) {
			return map[key]->value;
    	} else {
    		return std::nullopt;
    	}
    }

    Value get(Key key) {
        if (!this->map.count(key)) {
//			this->show();
			return -1;
        }
		_Node *node = this->map[key];
		this->to_head(node);
//         this->show();
        return node->value;
    }
    
    void put(Key key, Value value) {
        if (this->map.count(key)) {
			_Node *node = this->map[key];
			node->value = value;
			this->to_head(node);
	 	} else {
			if (this->used == 0) {
				_Node *node = new _Node();
				this->map[key] = node;
				node->key = key;
				node->value = value;

				this->used += 1;
				lru_tail = node;
				lru_head = node;
			} else if (this->used < this->capacity) {
				_Node *node = new _Node();
				this->map[key] = node;
				node->key = key;
				node->value = value;

				this->used += 1;
				node->next = lru_head;
				lru_head->last = node;
				lru_head = node;
			} else {
				this->map.erase(lru_tail->key);
				lru_tail->key = key;
				lru_tail->value = value;
				this->map[key] = lru_tail;
				this->to_head(lru_tail);
			}
		}
//		 this->show();
	}
};