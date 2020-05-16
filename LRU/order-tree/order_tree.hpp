#pragma once
#include <vector>
#include <queue>
#include <cstdio>
#include <memory>

using namespace std;

// NOTE: 有 enum 的話，可分成內部節點跟葉子節點
template<typename Value>
class DoublyLinkedNode {
public:
    shared_ptr<DoublyLinkedNode> children[2];
    int key; // 作用是 debug ? TODO: 若無用，刪除之
    Value value;          // value 若爲 -1 ，表示沒有 value
    int index;          // index 若爲 -1 ，表示其爲內部節點（而非葉子節點）
    DoublyLinkedNode() : children{nullptr, nullptr}, key(0), value(-1), index(-1) {}
    DoublyLinkedNode(int index) : children{nullptr, nullptr}, key(0), value(-1), index(index) {}
    DoublyLinkedNode(int index, int value) : children{nullptr, nullptr}, key(0), value(value), index(index) {}
    DoublyLinkedNode(int index, int key, int value) : children{nullptr, nullptr}, key(key), value(value), index(index) {}
};

template<typename Key, typename Value>
class OrderTree;

// OrderTree::get 的回傳結構
template<typename Key, typename Value>
struct GetRet {
    shared_ptr<OrderTree<Key, Value>> new_tree;
    shared_ptr<DoublyLinkedNode<Value>> new_node;
};

template<typename Key, typename Value>
struct KeyValue {
    Key key;
    Value value;
};

template<typename Key, typename Value>
class OrderTree {
	typedef DoublyLinkedNode<Value> _Node;
	typedef KeyValue<Key, Value> _KeyValue;
private:
    shared_ptr<OrderTree> new_tree() const {
        auto tree = make_shared<OrderTree>();
        tree->height = this->height;
        tree->root = nullptr;
        tree->cursor = this->cursor;
        return tree;
    }
	static shared_ptr<_Node> _createFullTree(int height, int index, vector<_KeyValue> &kvs) {
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
	static shared_ptr<_Node> createFullTree(int height, vector<_KeyValue> &kvs) {
		return _createFullTree(height, 0, kvs);
	}
	// 將 node 底下的葉子塞入 v 中。是 order 的輔助函數
	static void _order(shared_ptr<_Node> node, int height, deque<shared_ptr<_Node>> &v) {
    	if (height == 0) {
    		v.push_back(node);
    	} else {
    		if (node->children[0] != nullptr) {
    			_order(node->children[0], height - 1, v);
    		}
			if (node->children[1] != nullptr) {
				_order(node->children[1], height - 1, v);
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
			printf("(%d, %d) ", node->key, node->value);
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
	OrderTree(int height, vector<_KeyValue> &kvs) {
		this->height = height;
		this->cursor = kvs.size();
		this->root = createFullTree(height, kvs);
	}

	explicit OrderTree(int height) {
		this->height = height;
		this->cursor = 0;
		this->root = make_shared<_Node>();
	}

	// 打印所有葉子節點
	void show() {
		_show(this->root, this->height);
		printf("\n");
	}

	// 打印有東西的葉子順序
	void show_order() {
		printf("order: ");
		for (auto node: this->order()) {
			printf("(%d, %d)", node->key, node->value);
		}
		puts("");
	}

    // 創建一個新的樹，新增一個節點到當前 cursor 位置，其值爲 value
    pair<shared_ptr<OrderTree>, shared_ptr<_Node>> add(Value value) {
		auto old_pointer = this->root;     // 原樹的指標
		auto new_pointer = make_shared<_Node>();     // 正在創建的新樹的指標
		auto new_root = new_pointer;
		for (int h = this->height - 1; h >= 0; h--) {
			int br = this->cursor & (1 << h) ? 1 : 0;
			if (old_pointer != nullptr) {
				new_pointer->children[!br] = old_pointer->children[!br];
				old_pointer = old_pointer->children[br];
			}
			new_pointer->children[br] = make_shared<_Node>();
			new_pointer = new_pointer->children[br];
		}
		new_pointer->index = this->cursor;
		new_pointer->value = value;
		auto new_tree = this->new_tree();
		new_tree->root = new_root;
		new_tree->cursor++;
		pair<shared_ptr<OrderTree>, shared_ptr<_Node>> ret = {new_tree, new_pointer};
		return ret;
	}

	// 創建一個新的樹， node 的值被修改爲 value
	pair<shared_ptr<OrderTree>, shared_ptr<_Node>> change_value(shared_ptr<_Node> node, Value value) {
		auto old_pointer = this->root;      // 原樹的指標
		auto new_pointer = make_shared<_Node>();     // 正在創建的新樹的指標
		auto new_root = new_pointer;
		for (int h = this->height - 1; h >= 0; h--) {
			int br = node->index & (1 << h) ? 1 : 0;
			new_pointer->children[!br] = old_pointer->children[!br];
			old_pointer = old_pointer->children[br];
			new_pointer->children[br] = make_shared<_Node>();
			new_pointer = new_pointer->children[br];
		}
		new_pointer->value = value;
		new_pointer->index = node->index;
		shared_ptr<OrderTree> new_tree = this->new_tree();
		new_tree->root = new_root;
		pair<shared_ptr<OrderTree>, shared_ptr<_Node>> ret = {new_tree, new_pointer};
		return ret;
	}


	// 創建一個新的樹， node 將被移到當前 cursor 位置
	// to_head(A) 做兩件事情
	// 1. 先將 A 所在位置設爲空
	// 2. 在 cursor 位置填入 A
	//
	// 也就是說，我們必須複製通往 node->index 的路徑，以及通往 this->cursor 的路徑
	pair<shared_ptr<OrderTree>, shared_ptr<_Node>> to_head(shared_ptr<_Node> node) {
		if (this->cursor == 1 << this->height) {
			throw "樹已滿，不可再前放東西";
		}
		auto old_pointer = this->root;     // 原樹的指標
		auto pointer =  make_shared<_Node>();     // 正在創建的新樹的指標
		auto new_root = pointer;
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
			pointer->children[br] = make_shared<_Node>();
			pointer = pointer->children[br];
		}
		printf("node->index: %d, this->cursor: %d\n", node->index, this->cursor);
		printf("this->height: %d\n", this->height);
		printf("common_h: %d\n", common_h);

		// 處理 node->index
		// 先觀察何處開始爲孤枝
		int lone_h = 0; // 在 lone_h 高度時沒有分叉，且一路向下也沒有任何分叉
		auto p = old_pointer->children[0]; // 因爲 cursor 較新， node 必定在 cursor 之左，走 children[0] 分支
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
		auto index_pointer = pointer;
		p = old_pointer;
		int h;
		for (h = common_h; h > lone_h; h--) {
			int br = node->index & (1 << h) ? 1 : 0;
			index_pointer->children[!br] = p->children[!br];
			index_pointer->children[br] = make_shared<_Node>();
			index_pointer = index_pointer->children[br];
			p = p->children[br];
		}
		// 孤枝處設爲 nullptr
		int br = node->index & (1 << h) ? 1 : 0;
		index_pointer->children[!br] = p->children[!br];
		index_pointer->children[br] = nullptr;

		// 處理 this->cursor
		pointer->children[1] = make_shared<_Node>();
		auto cursor_pointer = pointer->children[1];
		p = old_pointer->children[1];
		for (h = common_h - 1; h >= 0 && p != nullptr; h--) {
			printf("cursor height: %d\n", h);
			int br = this->cursor & (1 << h) ? 1 : 0;
			cursor_pointer->children[!br] = p->children[!br];
			cursor_pointer->children[br] = make_shared<_Node>();
			cursor_pointer = cursor_pointer->children[br];
			p = p->children[br];
		}
		// 原樹中已經沒有同路徑的分支了
		for (; h >= 0; h--) {
			printf("走自己的路 height: %d\n", h);
			int br = this->cursor & (1 << h) ? 1 : 0;
			cursor_pointer->children[br] = make_shared<_Node>();
			cursor_pointer = cursor_pointer->children[br];
		}
		cursor_pointer->index = this->cursor;
		cursor_pointer->value = node->value;
		cursor_pointer->key = node->key;

		// 創建新的樹並回傳
		auto new_tree = this->new_tree();
		new_tree->root = new_root;
		new_tree->cursor++;
		pair<shared_ptr<OrderTree>, shared_ptr<_Node>> ret = {new_tree, cursor_pointer};
		return ret;
	}

	// 回傳現在葉子節點的順序
	deque<shared_ptr<_Node>> order() {
		deque<shared_ptr<_Node>> ret;
		_order(root, height, ret);
		return ret;
	}

	// 從 nodes 重建新樹，這些節點將緊密位於新樹左側
	shared_ptr<OrderTree> rebuild(deque<shared_ptr<_Node>> nodes) {
		auto tree = this->new_tree();
		tree->cursor = nodes.size();
		// 每個迴圈搞定一層
		for (int h = 0; h < height; h++) {
			int num = nodes.size();
			for (int i = 0; i + 1 < num; i += 2) {
				auto parent = make_shared<_Node>();
				parent->children[0] = nodes.front(); nodes.pop_front();
				parent->children[1] = nodes.front(); nodes.pop_front();
				nodes.push_back(parent);
			}
			if (num % 2 == 1) {
				auto parent = make_shared<_Node>();
				parent->children[0] = nodes.front(); nodes.pop_front();
				parent->children[1] = nullptr;
				nodes.push_back(parent);
			}
		}
		tree->root = nodes[0];
		return tree;
	}

	// 創建一個新的樹，若 cursor 已滿，會將所有節點緊密併到新樹左側
	// 否則，node 將被移到當前 cursor 位置
	GetRet<Key, Value> get(shared_ptr<_Node> node) {
		auto ret = this->to_head(node);
		auto new_tree = ret.first;

		if (new_tree->cursor == 1 << this->height) {
			printf("滿了！！！！！！！！！\n");
			auto order = new_tree->order();
			new_tree = rebuild(order);
			return GetRet<Key, Value> {
					new_tree,
					ret.second,
			};
		} else {
			return GetRet<Key, Value> {
					new_tree,
					ret.second,
			};
		}
	}

	// 創建一個新的樹， node 的值被修改爲 value ，並且 node 將被移到當前 cursor 位置
	// XXX: 函數簽名應爲 put(Value value)
	pair<shared_ptr<OrderTree>, shared_ptr<_Node>> put(shared_ptr<_Node> node, Value value) {
		auto ret = change_value(node, value);
		auto tree = ret.first;
		tree->show();
		auto new_node = ret.second;
		return tree->to_head(new_node);
	}
};