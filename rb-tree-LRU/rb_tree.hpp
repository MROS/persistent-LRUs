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
    immer::map<ID, shared_ptr<Node<IDPair<ID, V>>> *> map_;
	RedBlackTree(immer::map<ID, shared_ptr<Node<IDPair<ID, V>>> *> map): map_(map) { }
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
		Node<IDPair<ID, V>>::insert(&this->root_, key, pair, true, [this](auto node) {
			this->map_ = this->map_.set(node->get()->entry->value.id, node);
		});
		// TODO: 插入剛才新加的節點
	}
	void remove(int key) {
		if (freeze_) {
			throw "嘗試刪除一棵已凍結之紅黑樹的元素";
		}
		this->size_ -= 1;
		// TODO:
	}
	void remove_least() {
		if (freeze_) {
			throw "嘗試刪除一棵已凍結之紅黑樹對最小元素";
		}
		if (this->root_ == nullptr) {
			throw "嘗試刪除空紅黑樹的最小元素";
		}
		this->size_ -= 1;
		auto least_node = Node<IDPair<ID, V>>::get_least(this->root_);
		bool is_root = false;
		if (least_node->get() == this->root_.get()) {
			is_root = true;
		}
		Node<IDPair<ID, V>>::remove(least_node, least_node->get()->entry->key, is_root, [this](auto node) {
			this->map_ = this->map_.erase(node->get()->entry->value.id);
		});
	}
	shared_ptr<Node<IDPair<ID, V>>> *find_by_id(const ID &id) {
		return this->map_[id];
	}
};
