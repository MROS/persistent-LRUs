#include "../LRU.h"
#include <memory>
using namespace std;

template<typename Key, typename Value>
class HeapLRU : public LRU<Key, Value> {
public:
	typedef LRU<Key, Value> Parent;
	typedef std::variant<Get<Key>, Put<Key, Value>> Cmd;

	explicit HeapLRU(size_t capacity) {
	}
	std::shared_ptr<Parent> create(size_t capacity) {
		return nullptr;
	}
	std::optional<Value> read_only_get(Key &key) {
		return nullopt;
	}
	std::shared_ptr<Parent> batch_operate(std::vector<Cmd> &cmds) {
		return nullptr;
	}
};
