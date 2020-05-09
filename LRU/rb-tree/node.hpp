#pragma once
#include <memory>
#include <optional>
#include <functional>

using namespace std;

enum class Color { R, B };

template<typename T>
T* make_mut(shared_ptr<T> *rc) {
	if (rc->use_count() > 1) {
		// 注意：這是唯一會複製東西的地方！！
		auto inner = *(rc->get());
		*rc = make_shared<T>(inner);

	}
	return rc->get();
}

template<typename T>
shared_ptr<T> take(shared_ptr<T> *rc) {
	shared_ptr<T> ret = nullptr;
	ret.swap(*rc);
	return ret;
}

template<typename T>
struct Entry {
	int key;
	T value;
};

template<typename T>
class Node {
public:
	shared_ptr<Entry<T>> entry;
	Color color;
	shared_ptr<Node> right = {};
	shared_ptr<Node> left = {};

	string debug() const {
		string color_str{this->color == Color::B ? "黑" : "紅"};
		return "NODE: color: " + color_str + ", key: " + to_string(this->entry->key);
	}
	shared_ptr<Node<T>> clone() const {
		return make_shared<Node<T>>(*this);
	}

	Node(Color color, int key, const T &value) : entry(nullptr), color(color) {
		Entry<T> entry = { key, value };
		this->entry = make_shared<Entry<T>>(entry);
	}

	optional<Color> left_color() const {
		return this->left != nullptr ? this->left->color : optional<Color>{};
	}
	optional<Color> right_color() const {
		return this->right != nullptr ? this->right->color : optional<Color>{};
	}
	static shared_ptr<Node<T>>* get_least(shared_ptr<Node<T>>& node) {
		return node->left != nullptr ? Node<T>::get_least(node->left) : &node;
	}
	static shared_ptr<Node<T>> *get_node(shared_ptr<Node<T>>& node, int key) { // 要怎麼回傳 Option<&T> 啊 =_=
		if (key > node->entry->key) {
			if (node->right != nullptr) {
				return Node<T>::get_node(node->right, key);
			} else {
				return nullptr;
			}
		} else if (key < node->entry->key) {
			if (node->left != nullptr) {
				return Node<T>::get_node(node->left, key);
			} else {
				return nullptr;
			}
		} else {
			return &node;
		}
	}

    static void insert(
		shared_ptr<Node<T>> *shared_ptr_node,
		int key,
		const T &value,
		bool is_root
	) {
		if (shared_ptr_node->get() != nullptr) {
			auto node = make_mut(shared_ptr_node);
			if (key < node->entry->key) {
				Node::insert(&node->left, key, value, false);
				node->balance();
			} else if (key > node->entry->key) {
				Node::insert(&node->right, key, value, false);
				node->balance();
			} else {
				throw "插入重複的鍵！";
			}
			if (is_root) {
				node->color = Color::B;
			}
		} else {
			auto color = Color::R;
			if (is_root) {
				color = Color::B;
			}
			*shared_ptr_node = make_shared<Node<T>>(color, key, value);
		}
    }
	static void remove(
		shared_ptr<Node<T>> *shared_ptr_node,
		int key,
		bool is_root
	) {
		bool make_node_none = false;
		if (shared_ptr_node->get() != nullptr) {
			auto node = make_mut(shared_ptr_node);
			if (key > node->entry->key) {
				node->del_right(key);
			} else if (key < node->entry->key) {
				node->del_left(key);
			} else {
				auto left = take(&node->left);
				auto right = take(&node->right);
				make_node_none = !Node<T>::remove_fuse(node, left, right);
			}
			if (is_root) {
				node->color = Color::B;
			}
		} else {
			throw "移除時找不到節點";
		}
		if (make_node_none) {
			*shared_ptr_node = nullptr;
		}
	}

private:
	void balance() {
		if (this->color == Color::R) {
			return;
		}
		auto color_l = this->left_color();
		auto color_l_l = this->left != nullptr ? this->left->left_color() : optional<Color>{};
		auto color_l_r = this->left != nullptr ? this->left->right_color() : optional<Color>{};
		auto color_r = this->right_color();
		auto color_r_l = this->right != nullptr ? this->right->left_color() : optional<Color>{};
		auto color_r_r = this->right != nullptr ? this->right->right_color() : optional<Color>{};

		if (color_l == optional{Color::R} && color_l_l == optional{Color::R}) {
			auto node_l_ptr = take(&this->left);
			Node<T> *node_l = make_mut(&node_l_ptr);
			auto node_l_l_ptr = take(&node_l->left);
			Node<T> *node_l_l = make_mut(&node_l_l_ptr);

			this->color = Color::R;
			node_l->color = Color::B;
			node_l_l->color = Color::B;

			this->entry.swap(node_l->entry);
			node_l->left.swap(node_l->right);
			this->right.swap(node_l->right);
			
			this->left = node_l_l_ptr;
			this->right = node_l_ptr;
		} else if (color_l == optional{Color::R} && color_l_r == optional{Color::R}) {
			auto node_l_ptr = take(&this->left);
			Node<T> *node_l = make_mut(&node_l_ptr);
			auto node_l_r_ptr = take(&node_l->right);
			Node<T> *node_l_r = make_mut(&node_l_r_ptr);

			this->color = Color::R;
			node_l->color = Color::B;
			node_l_r->color = Color::B;

			this->entry.swap(node_l_r->entry);
			node_l_r->left.swap(node_l_r->right);
			node_l->right.swap(node_l_r->right);
			this->right.swap(node_l_r->right);

			this->right = node_l_r_ptr;
			this->left = node_l_ptr;
		} else if (color_r == optional{Color::R} && color_r_l == optional{Color::R}) {
			auto node_r_ptr = take(&this->right);
			Node<T> *node_r = make_mut(&node_r_ptr);
			auto node_r_l_ptr = take(&node_r->left);
			Node<T> *node_r_l = make_mut(&node_r_l_ptr);

			this->color = Color::R;
			node_r->color = Color::B;
			node_r_l->color = Color::B;

			this->entry.swap(node_r_l->entry);
			node_r->left.swap(node_r_l->right);
			node_r_l->left.swap(node_r_l->right);
			this->left.swap(node_r_l->left);

			this->left = node_r_l_ptr;
			this->right = node_r_ptr;
		} else if (color_r == optional{Color::R} && color_r_r == optional{Color::R}) {
			auto node_r_ptr = take(&this->right);
			Node<T> *node_r = make_mut(&node_r_ptr);
			auto node_r_r_ptr = take(&node_r->right);
			Node<T> *node_r_r = make_mut(&node_r_r_ptr);

			this->color = Color::R;
			node_r->color = Color::B;
			node_r_r->color = Color::B;

			this->entry.swap(node_r->entry);
			node_r->left.swap(node_r->right);
			this->left.swap(node_r->left);

			this->right = node_r_r_ptr;
			this->left = node_r_ptr;
		}
	}
	static bool remove_fuse(
		Node<T> *node,
		shared_ptr<Node<T>> &left,
		shared_ptr<Node<T>> &right
	) {
		if (left == nullptr && right == nullptr) {
			return false;
		} else if (left == nullptr && right != nullptr) {
			*node = *right;
			return true;
		} else if (left != nullptr && right == nullptr) {
			*node = *left;
			return true;
		} else {
			if (left->color == Color::B && right->color == Color::R) {
				auto r = make_mut(&right);
				auto rl = take(&r->left);
				// This will always return `true`.
				Node<T>::remove_fuse(node, left, rl);
				swap(*node, *r);
				node->left = right;
			} else if (left->color == Color::R && right->color == Color::B) {
				auto l = make_mut(&left);
				auto lr = take(&l->right);
				// This will always return `true`.
				Node<T>::remove_fuse(node, lr, right);
				swap(*node, *l);
				node->right = left;
			} else if (left->color == Color::R && right->color == Color::R) {
                auto r = make_mut(&right);
				auto rl = take(&r->left);
                auto l = make_mut(&left);
                auto lr = take(&l->right);
				auto fused = Node<T>::remove_fuse(node, lr, rl);
				if (node->color == Color::R) {
					if (fused) {
						auto fl = take(&node->left);
						auto fr = take(&node->right);
						l->right = fl;
						r->left = fr;
						node->left = left;
						node->right = right;
					}
				} else {
					swap(*node, *l);
					if (fused) {
						r->left = left;
					}
					node->right = right;
				}
			} else if (left->color == Color::B && right->color == Color::B) {
                auto r = make_mut(&right);
				auto rl = take(&r->left);
                auto l = make_mut(&left);
                auto lr = take(&l->right);
				auto fused = Node<T>::remove_fuse(node, lr, rl);
				if (node->color == Color::R) {
					if (fused) {
						auto fl = take(&node->left);
						auto fr = take(&node->right);
						l->right = fl;
						r->left = fr;
						node->left = left;
						node->right = right;
					}
				} else {
					swap(*node, *l);
					if (fused) {
						r->left = left;
					}
					node->right = right;

					node->color = Color::R;
					node->right = right;
					node->remove_balance_left();
				}
			}
			return true;
		}
    }
	void remove_balance() {
		auto l_color = this->left_color();
		auto r_color = this->right_color();
		if (l_color == optional<Color>{Color::R} && r_color == optional<Color>{Color::R}) {
			make_mut(&this->left)->color = Color::B;
			make_mut(&this->right)->color = Color::B;
			this->color = Color::R;
		} else {
			if (this->color != Color::B) {
				throw "不是黑色";
			}
			this->balance();
		}
    }
	
    void remove_balance_left() {
		auto color_l = this->left_color();
		auto color_r = this->right_color();
        auto color_r_l = this->right == nullptr ? optional<Color>{} : optional<Color>{this->right->left_color()};

		if (color_l == optional<Color>{Color::R}) {
			auto self_l = make_mut(&this->left);
			this->color = Color::R;
			self_l->color = Color::B;
		} else if (color_r == optional<Color>{Color::B}) {
			auto self_r = make_mut(&this->right);
			this->color = Color::B;
			self_r->color = Color::R;
			this->remove_balance();
		} else if (color_r == optional<Color>{Color::R} && color_r_l == optional<Color>{Color::B}) {
			auto self_r = make_mut(&this->right);
			auto self_r_l_ptr = take(&self_r->left);
			auto self_r_l = make_mut(&self_r_l_ptr);
			auto new_r_l = take(&self_r_l->right);

			self_r->color = Color::B;
			self_r->left = new_r_l;
			make_mut(&self_r->right)->color = Color::R;
			self_r->remove_balance();
			this->color = Color::R;
			self_r_l->right = take(&self_r_l->left);
			self_r_l->left = take(&this->left);
			swap(this->entry, self_r_l->entry);
			this->left = self_r_l_ptr;
		} else {
			throw "不應走到這裡= =";
		}
    }
    void remove_balance_right() {
		auto color_r = this->right_color();
		auto color_l = this->left_color();
        auto color_l_r = this->left == nullptr ? optional<Color>{} : optional<Color>{this->left->right_color()};

		if (color_r == optional<Color>{Color::R}) {
			auto self_r = make_mut(&this->right);
			this->color = Color::R;
			self_r->color = Color::B;
		} else if (color_l == optional<Color>{Color::B}) {
			auto self_l = make_mut(&this->left);
			this->color = Color::B;
			self_l->color = Color::R;
			this->remove_balance();
		} else if (color_l == optional<Color>{Color::R} && color_l_r == optional<Color>{Color::B}) {
			auto self_l = make_mut(&this->left);
			auto self_l_r_ptr = take(&self_l->right);
			auto self_l_r = make_mut(&self_l_r_ptr);
			auto new_l_r = take(&self_l_r->left);

			self_l->color = Color::B;
			self_l->left = new_l_r;
			make_mut(&self_l->left)->color = Color::R;
			self_l->remove_balance();
			this->color = Color::R;
			self_l_r->left = take(&self_l_r->right);
			self_l_r->right = take(&this->right);
			swap(this->entry, self_l_r->entry);
			this->right = self_l_r_ptr;
		} else {
			throw "不應走到這裡= =";
		}
    }
	
	void del_left(int key) {
		auto original_left_color = this->left_color();
		Node<T>::remove(&this->left, key, false);
		this->color = Color::R; // In case of rebalance the color does not matter.
		if (original_left_color == optional<Color>{Color::B}) {
			this->remove_balance_left();
		}
	}
	void del_right(int key) {
		auto original_right_color = this->right_color();
		Node<T>::remove(&this->right, key, false);
		this->color = Color::R; // In case of rebalance the color does not matter.
		if (original_right_color == optional<Color>{Color::B}) {
			this->remove_balance_right();
		}
	}
};