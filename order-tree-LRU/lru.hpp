#include "order_tree.hpp"
#include "../LRU.h"
#include <immer/map.hpp>
#include <optional>
#include <vector>
#include <memory>

template<typename Key, typename Value>
class OrderTreeLRU : public LRU<Key, Value> {
private:
    int capacity{};
    int used{};
    OrderTree<Key, Value> *order_tree{};
    immer::map<int, Node<Value> *> map;

public:
	typedef OrderTreeLRU<Key, Value> Self;
	typedef std::variant<Get<Key>, Put<Key, Value>> Cmd;

    explicit OrderTreeLRU(int capacity);
    OrderTreeLRU() = default;
    std::pair<OrderTreeLRU*, int> get(int key);
    OrderTreeLRU* put(int key, int value);

	std::optional<Value> read_only_get(Key &key) {
		return std::optional<Value>();
	}

	std::shared_ptr<Self> batch_operate(std::vector<Cmd> cmds) {
		return std::shared_ptr<Self>();
	}
};