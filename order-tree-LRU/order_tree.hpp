#pragma once
#include <vector>
#include <cstdio>

// NOTE: 有 enum 的話，可分成內部節點跟葉子節點
template<typename Value>
class Node {
public:
    Node *children[2];
    int key;
    Value value;          // value 若爲 -1 ，表示沒有 value
    int index;          // index 若爲 -1 ，表示其爲內部節點（而非葉子節點）
    Node() : children{nullptr, nullptr}, key(0), value(-1), index(-1) {}
    Node(int index) : children{nullptr, nullptr}, key(0), value(-1), index(index) {}
    Node(int index, int value) : children{nullptr, nullptr}, key(0), value(value), index(index) {}
    Node(int index, int key, int value) : children{nullptr, nullptr}, key(key), value(value), index(index) {}
};

template<typename Key, typename Value>
class OrderTree;

// OrderTree::get 的回傳結構
template<typename Key, typename Value>
struct GetRet {
    OrderTree<Key, Value> *new_tree;
    Node<Value> *new_node;
    std::vector<Node<Value>*> *reorder;
};

template<typename Key, typename Value>
struct KeyValue {
    Key key;
    Value value;
};

template<typename Key, typename Value>
class OrderTree {
	typedef Node<Value> _Node;
	typedef KeyValue<Key, Value> _KeyValue;
private:
    OrderTree *new_tree() const {
        auto *tree = new OrderTree();
        tree->height = this->height;
        tree->root = nullptr;
        tree->cursor = this->cursor;
        return tree;
    }
	static _Node *_createFullTree(int height, int index, std::vector<_KeyValue> &kvs) {
		if (height < 0) {
			throw "樹高度不得爲負";
		} else if (height == 0 && index < kvs.size()) {
			int value = kvs.size() > index ? kvs[index].value : -1;
			return new _Node(index, value);
		} else if (height == 0) {
			return nullptr;
		} else {
			auto root = new _Node();
			root->children[0] = _createFullTree(height - 1, index * 2, kvs);
			root->children[1] = _createFullTree(height - 1, index * 2 + 1, kvs);
			return root;
		}
	}
	static _Node *createFullTree(int height, std::vector<_KeyValue> &kvs) {
		return _createFullTree(height, 0, kvs);
	}
	static void _show(_Node *node, int height) {
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
			printf("(%d, %d) ", node->key, node->value);
		}
	}

public:
	// 元素數量爲 2^(height-1) ，只佔用一半的葉子節點
	int height;
    _Node *root;
    // cursor 代表當前最新葉子節點的索引，由左至右
    // 最左的葉子索引爲 0 ，最右的葉子索引爲 2^height - 1
    int cursor;
    OrderTree() = default;
	OrderTree(int height, std::vector<_KeyValue> &kvs) {
		this->height = height;
		this->cursor = kvs.size();
		this->root = createFullTree(height, kvs);
	}

	explicit OrderTree(int height) {
		this->height = height;
		this->cursor = 0;
		this->root = new _Node();
	}

	// 打印所有葉子節點
	void show() {
		_show(this->root, this->height);
		printf("\n");
	}

    // 創建一個新的樹，新增一個節點到當前 cursor 位置，其值爲 value
    std::pair<OrderTree*, _Node*> add(Value value) {
		_Node *old_pointer = this->root;     // 原樹的指標
		_Node *new_pointer = new _Node();     // 正在創建的新樹的指標
		_Node *new_root = new_pointer;
		for (int h = this->height - 1; h >= 0; h--) {
			int br = this->cursor & (1 << h) ? 1 : 0;
			if (old_pointer != nullptr) {
				new_pointer->children[!br] = old_pointer->children[!br];
				old_pointer = old_pointer->children[br];
			}
			new_pointer->children[br] = new _Node();
			new_pointer = new_pointer->children[br];
		}
		new_pointer->index = this->cursor;
		new_pointer->value = value;
		OrderTree *new_tree = this->new_tree();
		new_tree->root = new_root;
		new_tree->cursor++;
		std::pair<OrderTree*, _Node*> ret = {new_tree, new_pointer};
		return ret;
	}

	// 創建一個新的樹， node 的值被修改爲 value
	std::pair<OrderTree*, _Node*> change_value(_Node *node, Value value) {
		_Node *old_pointer = this->root;     // 原樹的指標
		_Node *new_pointer = new _Node();     // 正在創建的新樹的指標
		_Node *new_root = new_pointer;
		for (int h = this->height - 1; h >= 0; h--) {
			int br = node->index & (1 << h) ? 1 : 0;
			new_pointer->children[!br] = old_pointer->children[!br];
			old_pointer = old_pointer->children[br];
			new_pointer->children[br] = new _Node();
			new_pointer = new_pointer->children[br];
		}
		new_pointer->value = value;
		OrderTree *new_tree = this->new_tree();
		new_tree->root = new_root;
		std::pair<OrderTree*, _Node*> ret = {new_tree, new_pointer};
		return ret;
	}


	// 創建一個新的樹， node 將被移到當前 cursor 位置
	// to_head(A) 做兩件事情
	// 1. 先將 A 所在位置設爲空
	// 2. 在 cursor 位置填入 A
	//
	// 也就是說，我們必須複製通往 node->index 的路徑，以及通往 this->cursor 的路徑
	std::pair<OrderTree*, _Node*> to_head(_Node *node) {
		if (this->cursor == 1 << this->height) {
			throw "樹已滿，不可再前放東西";
		}
		_Node *old_pointer = this->root;     // 原樹的指標
		_Node *pointer = new _Node();     // 正在創建的新樹的指標
		_Node *new_root = pointer;
		// 一直到 node->index 跟 this->cursor 的最低共同祖先爲止
		// 目標都是一樣的，都是複製這條路徑
		int common_h;
		for (common_h = this->height - 1;
			 common_h >= 0 && (node->index & (1 << common_h)) == (this->cursor & (1 << common_h));
			 common_h--)
		{
			int br = this->cursor & (1 << common_h) ? 1 : 0;
			pointer->children[!br] = old_pointer->children[!br];
			old_pointer = old_pointer->children[br];
			pointer->children[br] = new _Node();
			pointer = pointer->children[br];
		}
		printf("node->index: %d, this->cursor: %d\n", node->index, this->cursor);
		printf("this->height: %d\n", this->height);
		printf("common_h: %d\n", common_h);

		// 處理 node->index
		// 先觀察何處開始爲孤枝
		int lone_h = 0; // 在 lone_h 高度時沒有分叉，且一路向下也沒有任何分叉
		_Node *p = old_pointer->children[0]; // 因爲 cursor 較新， node 必定在 cursor 之左，走 children[0] 分支
		for (int h = common_h - 1; h >= 0; h--) {
			int br = node->index & (1 << h) ? 1 : 0;
			if (p->children[!br] == nullptr) {
				lone_h = lone_h < 0 ? h + 1 : lone_h;
				printf("孤枝高度可能爲 %d\n", lone_h);
			} else {
				lone_h = 0;
			}
			p = p->children[br];
		}
		printf("孤枝高度爲 %d\n", lone_h);

		// 僅複製到孤枝分叉處

		// 若最低共同祖先一往左就是孤枝，整條設爲 nullptr
		_Node *index_pointer = pointer;
		p = old_pointer;
		int h;
		for (h = common_h; h > lone_h; h--) {
			int br = node->index & (1 << h) ? 1 : 0;
			index_pointer->children[!br] = p->children[!br];
			index_pointer->children[br] = new _Node();
			index_pointer = index_pointer->children[br];
			p = p->children[br];
		}
		// 孤枝處設爲 nullptr
		int br = node->index & (1 << h) ? 1 : 0;
		index_pointer->children[!br] = p->children[!br];
		index_pointer->children[br] = nullptr;

		// 處理 this->cursor
		pointer->children[1] = new _Node();
		_Node *cursor_pointer = pointer->children[1];
		p = old_pointer->children[1];
		for (h = common_h - 1; h >= 0 && p != nullptr; h--) {
			printf("cursor height: %d\n", h);
			int br = this->cursor & (1 << h) ? 1 : 0;
			cursor_pointer->children[!br] = p->children[!br];
			cursor_pointer->children[br] = new _Node();
			cursor_pointer = cursor_pointer->children[br];
			p = p->children[br];
		}
		// 原樹中已經沒有同路徑的分支了
		for (; h >= 0; h--) {
			printf("走自己的路 height: %d\n", h);
			int br = this->cursor & (1 << h) ? 1 : 0;
			cursor_pointer->children[br] = new _Node();
			cursor_pointer = cursor_pointer->children[br];
		}
		cursor_pointer->index = this->cursor;
		cursor_pointer->value = node->value;
		cursor_pointer->key = node->key;

		// 創建新的樹並回傳
		OrderTree *new_tree = this->new_tree();
		new_tree->root = new_root;
		new_tree->cursor++;
		std::pair<OrderTree*, _Node*> ret = {new_tree, cursor_pointer};
		return ret;
	}

	// 回傳現在葉子節點的順序
	std::vector<_KeyValue> *order(_Node *node);

	// 創建一個新的樹，若 cursor 已滿，會將所有節點緊密併到新樹左側
	// 否則，node 將被移到當前 cursor 位置
	GetRet<Key, Value> get(_Node *node) {
		auto ret = this->to_head(node);
		OrderTree *new_tree = ret.first;

		if (new_tree->cursor == 1 << this->height) {
			printf("滿了！！！！！！！！！\n");
			return GetRet<Key, Value> {
					new_tree,
					ret.second,
					nullptr
			};
		} else {
			return GetRet<Key, Value> {
					new_tree,
					ret.second,
					nullptr
			};
		}
	}

	// 創建一個新的樹， node 的值被修改爲 value ，並且 node 將被移到當前 cursor 位置
	std::pair<OrderTree*, _Node*> put(_Node *node, Value value);

    OrderTree* update(_Node *node, Value value);
};