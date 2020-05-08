#pragma once
#include "node.hpp"

#include <immer/map.hpp>

template<typename ID, typename V>
struct IDPair {
	ID id;
	V value;
};

// 帶雜湊表的持久化紅黑樹，紅黑樹的鍵是 `int`，雜湊表的主鍵是 `ID`
template<typename ID, typename V>
class RedBlackTree {
private:
	shared_ptr<Node<IDPair<ID, V>>> root_ = nullptr;
	size_t size_ = 0;
	bool freeze_ = false;
    immer::map<ID, int> map_;
	RedBlackTree(immer::map<ID, int> map): map_(map) { }
public:
	RedBlackTree() { }
	RedBlackTree clone_and_freeze() {
		RedBlackTree<ID, V> new_tree{ this->map_ };
		if (this->root_ != nullptr) {
			new_tree.root_ = this->root_->clone();
		}
		new_tree.size_ = this->size_;
		this->freeze_ = true;
		return new_tree;
	}
	void insert(int key, ID id, V value) {
		if (freeze_) {
			throw "嘗試插入一棵已凍結之紅黑樹";
		}
		this->size_ += 1;
		IDPair<ID, V> pair = { id, value };
		Node<IDPair<ID, V>>::insert(&this->root_, key, pair, true);
		this->map_ = this->map_.set(id, key);
	}
	void remove(shared_ptr<Node<IDPair<ID, V>>> *node) {
		if (freeze_) {
			throw "嘗試刪除一棵已凍結之紅黑樹的元素";
		}
		this->size_ -= 1;

		bool is_root = false;
		if (node->get() == this->root_.get()) {
			is_root = true;
		}
		this->map_ = this->map_.erase(node->get()->entry->value.id);
		Node<IDPair<ID, V>>::remove(node, node->get()->entry->key, is_root);
	}
	void remove_least() {
		if (this->root_ == nullptr) {
			throw "嘗試刪除空紅黑樹的最小元素";
		}
		auto least_node = Node<IDPair<ID, V>>::get_least(this->root_);
		this->remove(least_node);
	}
	shared_ptr<Node<IDPair<ID, V>>> *find_by_id(const ID &id) {
		auto key = this->map_[id];
		return Node<IDPair<ID, V>>::get_node(this->root_, key);
	}
};
