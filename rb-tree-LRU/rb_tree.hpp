#pragma once
#include "node.hpp"

// #include <immer/map.hpp>

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
public:
	RedBlackTree() {
		// TODO: 初始化雜湊表
	}
	RedBlackTree clone_and_freeze() {
		RedBlackTree<ID, V> new_tree{};
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
		Node<IDPair<ID, V>>::insert(&this->root_, key, pair, true, [](auto node) {

		});
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
		Node<IDPair<ID, V>>::remove(least_node, least_node->get()->entry->key, is_root, [](const Node<IDPair<ID, V>> &node) {

		});
	}
	Entry<IDPair<ID, V>> *find_by_id() {
		//
	}
};
