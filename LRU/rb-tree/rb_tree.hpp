#pragma once
#include "node.hpp"
#include <iostream>

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
	int size() const {
		return this->size_;
	}
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
	void remove(ID id) {
		if (freeze_) {
			throw "嘗試刪除一棵已凍結之紅黑樹的元素";
		}
		this->size_ -= 1;

		auto node = this->find_by_id(id)->get();
		Node<IDPair<ID, V>>::remove(&this->root_, node->entry->key, true);
		this->map_ = this->map_.erase(id);
	}
	void remove_least() {
		if (this->root_ == nullptr) {
			throw "嘗試刪除空紅黑樹的最小元素";
		}
		auto least_node = Node<IDPair<ID, V>>::get_least(this->root_)->get();
		this->remove(least_node->entry->value.id);
	}
	// TODO: 改成 find_key_by_id
	shared_ptr<Node<IDPair<ID, V>>> *find_by_id(const ID &id) {
		auto key = this->map_.find(id);
		if (key == nullptr) {
			return nullptr;
		} else {
			return Node<IDPair<ID, V>>::get_node(this->root_, *key);
		}
	}
};
