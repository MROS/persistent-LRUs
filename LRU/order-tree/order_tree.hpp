#pragma once
#include <vector>
#include <queue>
#include <cstdio>
#include <memory>
#include "../util.hpp"

using namespace std;

// NOTE: 有 enum 的話，可分成內部節點跟葉子節點
template<typename Key, typename Value>
class OrderTreeNode {
public:
    shared_ptr<OrderTreeNode> children[2];
    Key key;
    Value value;          // value 若爲 -1 ，表示沒有 value
    int index;          // index 若爲 -1 ，表示其爲內部節點（而非葉子節點）
    OrderTreeNode() : children{nullptr, nullptr}, key(0), value(-1), index(-1) {}
    OrderTreeNode(int index) : children{nullptr, nullptr}, key(0), value(-1), index(index) {}
    OrderTreeNode(int index, Value value) : children{nullptr, nullptr}, key(0), value(value), index(index) {}
    OrderTreeNode(int index, Key key, Value value) : children{nullptr, nullptr}, key(key), value(value), index(index) {}
};

template<typename Key, typename Value>
class OrderTree;

// OrderTree::get 的回傳結構
template<typename Key, typename Value>
struct GetRet {
    OrderTreeNode<Key, Value>* new_node;
	shared_ptr<vector<OrderTreeNode<Key, Value>*>> reorder;
};

// OrderTree::put 的回傳結構
template<typename Key, typename Value>
struct PutRet {
	OrderTreeNode<Key, Value>* new_node;
	optional<Key> deleted_key;
	shared_ptr<vector<OrderTreeNode<Key, Value>*>> reorder;
};

template<typename Key, typename Value>
class OrderTree {
	typedef OrderTreeNode<Key, Value> _Node;
private:
    shared_ptr<OrderTree> new_tree() const {
        auto tree = make_shared<OrderTree>();
        tree->height = this->height;
        tree->root = nullptr;
        tree->cursor = this->cursor;
        return tree;
    }
	static _Node* get_leftest_leaf(shared_ptr<_Node> node, int height) {
    	if (node == nullptr) {
    		return nullptr;
    	}
    	if (height == 0) {
    		return node.get();
    	} else {
    		auto leaf = get_leftest_leaf(node->children[0], height - 1);
			if (leaf != nullptr) {
				return leaf;
			}
			leaf = get_leftest_leaf(node->children[1], height - 1);
			if (leaf != nullptr) {
				return leaf;
			}
			return nullptr;
		}
	}
	// 將 node 底下的葉子塞入 v 中。是 order 的輔助函數
	static void _order(shared_ptr<_Node> *node, int height, shared_ptr<deque<shared_ptr<_Node>>> v) {
    	if (height == 0) {
    		v->push_back(make_shared<_Node>(**node));
		} else {
    		auto children = (*node)->children;
    		if (children[0] != nullptr) {
    			_order(&children[0], height - 1, v);
    		}
			if (children[1] != nullptr) {
				_order(&children[1], height - 1, v);
			}
		}
	}
	static void _show(shared_ptr<_Node> node, int height) {
		if (node == nullptr) {
			for (int i = 0; i < (1 << height); i++) {
				printf("(空, 空) ");
			}
		} else if (height > 0) {
			// 爲內部節點
			_show(node->children[0], height - 1);
			_show(node->children[1], height - 1);
		} else {
			// 爲葉子節點
			printf("(%d, %d, %d) ", node->key, node->value, node->index);
		}
	}

public:
	// 元素數量爲 2^(height-1) ，只佔用一半的葉子節點
	int height;
    shared_ptr<_Node> root;
    // cursor 代表當前最新葉子節點的索引，由左至右
    // 最左的葉子索引爲 0 ，最右的葉子索引爲 2^height - 1
    int cursor;
    OrderTree() = default;

	explicit OrderTree(int height) {
		this->height = height;
		this->cursor = 0;
		this->root = make_shared<_Node>();
	}
	// transient
	shared_ptr<OrderTree> new_version() const {
		auto tree = make_shared<OrderTree>();
		tree->height = height;
		tree->root = make_shared<_Node>();
		// 共享兒子
		tree->root->children[0] = root->children[0];
		tree->root->children[1] = root->children[1];
		tree->cursor = cursor;
		return tree;
	}

	// 打印所有葉子節點
	void show() {
		_show(this->root, this->height);
		printf("\n");
	}

	// 回傳現在葉子節點的順序
	shared_ptr<deque<shared_ptr<_Node>>> get_order() {
		auto ret = make_shared<deque<shared_ptr<_Node>>>();
		_order(&root, height, ret);
		return ret;
	}

	// 打印有東西的葉子順序
	void show_order() {
		auto order = get_order();
		for (auto node: *order) {
			printf("(%d, %d)", node->key, node->value);
		}
		puts("");
	}

    // 創建一個新的樹，新增一個節點到當前 cursor 位置，其值爲 value
    _Node* add(Key key, Value value) {
		_Node* cur = make_mut(&this->root);
		for (int h = this->height - 1; h >= 0; h--) {
			int br = this->cursor & (1 << h) ? 1 : 0;
			if (cur->children[br] == nullptr) {
				cur->children[br] = make_shared<_Node>();
				cur = cur->children[br].get();
			} else {
				cur = make_mut(&cur->children[br]);
			}
		}
		cur->index = this->cursor++;
		cur->key = key;
		cur->value = value;
		return cur;
	}

	// 創建一個新的樹， node 的值被修改爲 value, 鍵被修改爲 key
	_Node* change_node(_Node* node, Key key, Value value) {
		_Node* cur = make_mut(&this->root);
		for (int h = this->height - 1; h >= 0; h--) {
			int br = node->index & (1 << h) ? 1 : 0;
			if (cur->children[br] == nullptr) {
//				cur->children[br] = make_shared<_Node>();
//				cur = cur->children[br].get();
				show();
				printf("嘗試 change_node 不存在的節點 (%d, %d, %d)\n", node->key, node->value, node->index);
				throw "嘗試 change_node 不存在的節點";
			} else {
				cur = make_mut(&cur->children[br]);
			}
		}
		cur->key = key;
		cur->value = value;
		return cur;
	}

	void remove(shared_ptr<_Node> *node, int index, int h) {
		if (index < 0 || index > 1 << height) {
			show();
			printf("remove 超出範圍 index: %d\n", index);
			throw "remove 超出範圍 index";
		}
		if (*node == nullptr) {
			printf("remove 空指針\n");
			throw "remove 空指針";
		}
		_Node *cur = make_mut(node);
		if (h >= 0) {
			int br = index & (1 << h) ? 1 : 0;
			remove(&cur->children[br], index, h - 1);
			if (cur->children[br] == nullptr && cur->children[!br] == nullptr && (*node) != root) {
				take(node);
			}
		} else {
			take(node);
		}
	}

	// 創建一個新的樹， node 將被移到當前 cursor 位置
	// to_head(A) 做兩件事情
	// 1. 先將 A 所在位置設爲空
	// 2. 在 cursor 位置填入 A
	_Node* to_head(_Node* node) {
		if (this->cursor == 1 << this->height) {
			throw "樹已滿，不可再前放東西";
		}

		Key key = node->key;
		Value value = node->value;

		remove(&root, node->index, height - 1);
		return add(key, value);
	}

	// 從 nodes 重建新樹，這些節點將緊密位於新樹左側
	// nodes 將變爲新樹的節點，由左到右
	shared_ptr<vector<_Node*>> rebuild() {
//		show();
//		show_order();
//		printf("全複製\n");
		auto nodes = get_order();
		cursor = nodes->size();
		auto ret = make_shared<vector<_Node*>>();
		for (int i = 0; i < nodes->size(); i++) {
			(*nodes)[i]->index = i;
			ret->push_back((*nodes)[i].get());
		}
		// 每個迴圈搞定一層
		for (int h = 0; h < height; h++) {
			int num = nodes->size();
			for (int i = 0; i + 1 < num; i += 2) {
				auto parent = make_shared<_Node>();
				parent->children[0] = nodes->front(); nodes->pop_front();
				parent->children[1] = nodes->front(); nodes->pop_front();
				nodes->push_back(parent);
			}
			if (num % 2 == 1) {
				auto parent = make_shared<_Node>();
				parent->children[0] = nodes->front(); nodes->pop_front();
				parent->children[1] = nullptr;
				nodes->push_back(parent);
			}
		}
		root = (*nodes)[0];
//		show();
//		show_order();
		return ret;
	}

	// 創建一個新的樹，若 cursor 已滿，會將所有節點緊密併到新樹左側
	// 否則，node 將被移到當前 cursor 位置
	GetRet<Key, Value> get(_Node *node) {
		_Node *new_node = to_head(node);

		if (cursor == 1 << height) {
			auto order = rebuild();
			return GetRet<Key, Value> {
				new_node,
				order
			};
		} else {
			return GetRet<Key, Value> {
				new_node,
				nullptr
			};
		}
	}

	// 創建一個新的樹， node 的值被修改爲 value ，並且 node 將被移到當前 cursor 位置
	// 若 node 爲 nullptr ，則拔除最左側節點，加入一個新節點
	PutRet<Key, Value> put(_Node *node, Key key, Value value) {
		optional<Key> deleted = nullopt;
		if (node == nullptr) {
			_Node *leftest = get_leftest_leaf(root, height);
			node = leftest;
			deleted = leftest->key;
		}
		_Node *new_node = change_node(node, key, value);
		new_node = to_head(new_node);
		if (cursor == 1 << this->height) {
			auto order = rebuild();
			return {
				.new_node = nullptr,
				.deleted_key = deleted,
				.reorder = order
			};
		} else {
			return {
				.new_node = new_node,
				.deleted_key = deleted,
				.reorder = nullptr
			};
		}
	}
};