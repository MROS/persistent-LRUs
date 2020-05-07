#pragma once
#include <vector>
#include <queue>
#include <iostream>
#include <memory>
#include <optional>
#include <functional>

using namespace std;

enum class Color { R, B };

template<typename T>
T* make_mut(shared_ptr<T> *rc, function<void(const T&)> on_clone) {
	if (rc->use_count() > 1) {
		// 注意：這是唯一會複製東西的地方！！
		auto inner = *(rc->get());
		on_clone(inner);
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
private:
	void balance(function<void(const Node<T>&)> on_clone) {
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
			shared_ptr<Node<T>> node_l_ptr = this->left;
			this->left = nullptr;
			Node<T> *node_l = make_mut(&node_l_ptr, on_clone);
			shared_ptr<Node<T>> node_l_l_ptr = node_l->left;
			node_l->left = nullptr;
			Node<T> *node_l_l = make_mut(&node_l_l_ptr, on_clone);

			this->color = Color::R;
			node_l->color = Color::B;
			node_l_l->color = Color::B;

			this->entry.swap(node_l->entry);
			node_l->left.swap(node_l->right);
			this->right.swap(node_l->right);
			
			this->left = node_l_l_ptr;
			this->right = node_l_ptr;
		} else if (color_l == optional{Color::R} && color_l_r == optional{Color::R}) {
			shared_ptr<Node<T>> node_l_ptr = this->left;
			this->left = nullptr;
			Node<T> *node_l = make_mut(&node_l_ptr, on_clone);
			shared_ptr<Node<T>> node_l_r_ptr = node_l->right;
			node_l->right = nullptr;
			Node<T> *node_l_r = make_mut(&node_l_r_ptr, on_clone);

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
			shared_ptr<Node<T>> node_r_ptr = this->right;
			this->right = nullptr;
			Node<T> *node_r = make_mut(&node_r_ptr, on_clone);
			shared_ptr<Node<T>> node_r_l_ptr = node_r->left;
			node_r->left = nullptr;
			Node<T> *node_r_l = make_mut(&node_r_l_ptr, on_clone);

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
			shared_ptr<Node<T>> node_r_ptr = this->right;
			this->right = nullptr;
			Node<T> *node_r = make_mut(&node_r_ptr, on_clone);
			shared_ptr<Node<T>> node_r_r_ptr = node_r->right;
			node_r->right = nullptr;
			Node<T> *node_r_r = make_mut(&node_r_r_ptr, on_clone);

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
	shared_ptr<Entry<T>> get_entry(int key) const { // 要怎麼回傳 Option<&T> 啊 =_=
		if (key > this->entry->key) {
			if (this->right != nullptr) {
				return this->right->get_entry(key);
			} else {
				return nullptr;
			}
		} else if (key < this->entry->key) {
			if (this->left != nullptr) {
				return this->left->get_entry(key);
			} else {
				return nullptr;
			}
		} else {
			return this->entry;
		}
	}

    static bool insert(
		shared_ptr<Node<T>> *shared_ptr_node,
		int key,
		const T &value,
		bool is_root,
		function<void(const Node<T>&)> on_clone
	) {
		if (shared_ptr_node->get() != nullptr) {
			auto node = make_mut(shared_ptr_node, on_clone);
			if (key < node->entry->key) {
				Node::insert(&node->left, key, value, false, on_clone);
				node->balance(on_clone);
			} else if (key > node->entry->key) {
				Node::insert(&node->right, key, value, false, on_clone);
				node->balance(on_clone);
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
	bool remove_fuse(
		Node<T> *node,
		shared_ptr<Node<T>> left,
		shared_ptr<Node<T>> right,
		function<void(const Node<T>&)> on_clone
	) {
		if (left == nullptr && right == nullptr) {
			return false;
		} else if (left == nullptr && right != nullptr) {
			*node = *right;
			return true;
		} else if (left != nullptr && right == nullptr) {
			*node = *left;
		} else {
			if (left->color() == Color::B && right->color() == Color::R) {
				auto r = make_mut(&right, on_clone);
				auto rl = take(&r->left);

				// This will always return `true`.
				Node<T>::remove_fuse(node, left, rl);

				// swap(node, r);

				// node.left = Some(r_ptr);
			}
		}
        // match (left, right) {
        //     (Some(mut l_ptr), Some(mut r_ptr)) => {
        //         match (l_ptr.color, r_ptr.color) {
        //             (B, R) => {
        //             }
        //             (R, B) => {
        //                 let l = SharedPointer::make_mut(&mut l_ptr);
        //                 let lr = l.right.take();

        //                 // This will always return `true`.
        //                 Node::remove_fuse(node, lr, Some(r_ptr));

        //                 swap(node, l);

        //                 node.right = Some(l_ptr);
        //             }
        //             (R, R) => {
        //                 let r = SharedPointer::make_mut(&mut r_ptr);
        //                 let rl = r.left.take();
        //                 let l = SharedPointer::make_mut(&mut l_ptr);
        //                 let lr = l.right.take();

        //                 let fused = Node::remove_fuse(node, lr, rl);

        //                 match node.color {
        //                     R if fused => {
        //                         let fl = node.left.take();
        //                         let fr = node.right.take();

        //                         l.right = fl;
        //                         r.left = fr;

        //                         node.left = Some(l_ptr);
        //                         node.right = Some(r_ptr);
        //                     }
        //                     _ => {
        //                         swap(l, node);

        //                         if fused {
        //                             r.left = Some(l_ptr);
        //                         }

        //                         node.right = Some(r_ptr);
        //                     }
        //                 }
        //             }
        //             (B, B) => {
        //                 let r = SharedPointer::make_mut(&mut r_ptr);
        //                 let rl = r.left.take();
        //                 let l = SharedPointer::make_mut(&mut l_ptr);
        //                 let lr = l.right.take();

        //                 let fused = Node::remove_fuse(node, lr, rl);

        //                 match node.color {
        //                     R if fused => {
        //                         let fl = node.left.take();
        //                         let fr = node.right.take();

        //                         l.right = fl;
        //                         r.left = fr;

        //                         node.left = Some(l_ptr);
        //                         node.right = Some(r_ptr);
        //                     }
        //                     _ => {
        //                         swap(l, node);

        //                         if fused {
        //                             r.left = Some(l_ptr);
        //                         }

        //                         node.color = Color::Red;
        //                         node.right = Some(r_ptr);

        //                         node.remove_balance_left();
        //                     }
        //                 }
        //             }
        //         };

        //         true
        //     }
        // }
    }
};