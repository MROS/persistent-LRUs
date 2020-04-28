#pragma once
#include <optional>
#include <memory>
#include <variant>
#include <vector>

template<typename Key>
struct Get {
	Key key;
};

template<typename Key, typename Value>
struct Put {
	Key key;
	Value value;
};

template<typename Key, typename Value>
class LRU {
	int capacity{};
public:
	typedef std::variant<Get<Key>, Put<Key, Value>> Cmd;

	explicit LRU(size_t capacity) {
		this->capacity = capacity;
	}

	virtual std::optional<Value> read_only_get(Key &key) = 0;
//	virtual std::pair<std::optional<Value>, std::shared_ptr<LRU>> get(Key &key) = 0;
//	virtual std::shared_ptr<LRU> put(Key &key, Value &value) = 0;
	virtual std::shared_ptr<LRU> batch_operate(std::vector<Cmd> &cmds) = 0;
};
