#include <immer/map.hpp>
#include "order_tree.hpp"
#include "lru.hpp"

using namespace std;

template<typename Key, typename Value>
OrderTreeLRU<Key, Value>::OrderTreeLRU(int capacity) {
    this->capacity = capacity;
    this->used = 0;
    int h = 0, c = capacity;
    while (c > 0) {
        c /= 2;
        h++;
    }
    this->order_tree = new OrderTree<Key, Value>(h + 1);
    this->map = immer::map<int, Node<Value>*>{};
}

template<typename Key, typename Value>
pair<OrderTreeLRU<Key, Value>*, int> OrderTreeLRU<Key, Value>::get(int key) {
    Node<Value> *node = map[key];
    if (node == nullptr) {
        return make_pair(this, -1);
    }
    auto new_tree = order_tree->to_head(node);
    auto new_cache = new OrderTreeLRU<Key, Value>();
    new_cache->capacity = capacity;
    new_cache->used = used;
    new_cache->order_tree = new_tree.first;
    new_cache->map = this->map.set(key, new_tree.second);

    return make_pair(new_cache, node->value);
}

template<typename Key, typename Value>
OrderTreeLRU<Key, Value>* OrderTreeLRU<Key, Value>::put(int key, int value) {
    return nullptr;
}
