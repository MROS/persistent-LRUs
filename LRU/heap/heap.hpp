#pragma once
#include <memory>
#include <string>
#include <queue>
#include <cstdio>

using namespace std;

template<typename Key>
class HeapNode {
public:
	Key key;
	int timestamp;
	shared_ptr<HeapNode> children[2];
	HeapNode(Key key, int timestamp): key(key), timestamp(timestamp) {
		children[0] = nullptr;
		children[1] = nullptr;
	};
};

template<typename Key>
class Heap {
public:
	typedef HeapNode<Key> _Node;
private:
	void shiftdown(shared_ptr<_Node> node) {
		if (node == nullptr) {
			throw string("無法下移 nullptr");
		}
		if (node->children[0] == nullptr && node->children[1] == nullptr) {
			return;
		}

		// 若有子節點，找出時間戳較小的
		shared_ptr<_Node> small_child = node->children[0];
		if (node->children[1] != nullptr &&
			node->children[1]->timestamp < small_child->timestamp) {
			small_child = node->children[1];
		}

		// 若子節點時間戳小於父節點，交換父子節點
		if (small_child->timestamp < node->timestamp) {
			swap(node->key, small_child->key);
			swap(node->timestamp, small_child->timestamp);
			// 向下遞迴
			shiftdown(small_child);
		}
	}
	// NOTE: 可用 32 - __builtin_clz(x) 優化
	static int high_bit(int x) {
		int i;
		for (i = 0; i < 30; i++) {
			if ((1 << i) > x) { break; }
		}
//		printf("x = %d, high_bit(x) = %d\n", x, i - 1);
		return i - 1;
	}
public:
	shared_ptr<_Node> root;
	int capacity;
	int current_timestamp;
	Heap() = default;
	Heap(int capacity) : root(nullptr), capacity(capacity), current_timestamp(1) {}
	shared_ptr<Heap> new_version() {
		shared_ptr<Heap> ret = make_shared<Heap>();
		ret->capacity = capacity;
		ret->current_timestamp = current_timestamp;
		if (root == nullptr) {
			ret->root = nullptr;
		} else {
			ret->root = make_shared<_Node>();
			ret->root->children[0] = root->children[0];
			ret->root->children[1] = root->children[1];
		}
		return ret;
	}
	void show() {
		if (root == nullptr) { return; }
		queue<shared_ptr<_Node>> q;
		q.push(root);
		while (!q.empty()) {
			int len = q.size();
			for (int i = 0; i < len; i++) {
				auto node = q.front(); q.pop();
				if (node->children[0] != nullptr) {
					q.push(node->children[0]);
				}
				if (node->children[1] != nullptr) {
					q.push(node->children[1]);
				}
				printf("%d ", node->timestamp);
			}
			puts("");
		}
	}
	// 快取未滿時插入節點
	void add(Key key) {
		if (root == nullptr) {
			root = make_shared<_Node>(key, current_timestamp++);
		} else {
			int h = high_bit(current_timestamp) - 1;
			shared_ptr<_Node> *cur = &root;
			while (h >= 0) {
				int br = (current_timestamp >> h) & 1;
				cur = &((*cur)->children[br]);
				h--;
			}
			*cur = make_shared<_Node>(key, current_timestamp++);
		}
	}
};
