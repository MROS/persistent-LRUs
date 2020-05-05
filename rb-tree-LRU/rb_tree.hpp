#pragma once
#include <vector>
#include <queue>
#include <cstdio>
#include <memory>
#include <optional>
#include <functional>

using namespace std;

enum class Color { R, B };

// TODO: 在這個函式裡加上回調，讓外面可以得知誰被修改了
template<typename T>
T* make_mut(shared_ptr<T> *rc) {
	if (rc->use_count() == 1) {
		// 注意：這是唯一會複製東西的地方！！
		*rc = make_shared<T>(*(rc->get()));
	}
	return rc->get();
}

template<typename T>
struct Entry {
	int key;
	T value;
};

template<typename T>
class Node {
private:
	shared_ptr<Entry<T>> entry;
	Color color;
	shared_ptr<Node> right = {};
	shared_ptr<Node> left = {};

public:
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
	optional<shared_ptr<Entry<T>>> get_entry(int key) const { // 要怎麼回傳 Option<&T> 啊 =_=
		if (key > this->entry->key) {
			if (this->right != nullptr) {
				return this->right->get_entry(key);
			} else {
				return {};
			}
		} else if (key < this->entry->key) {
			if (this->left != nullptr) {
				return this->left->get_entry(key);
			} else {
				return {};
			}
		} else {
			return this->entry;
		}
	}

	void balance() {
		// match self.color {
        //     B => {
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
			Node<T> *node_l = make_mut(&node_l_ptr);
			shared_ptr<Node<T>> node_l_l_ptr = node_l->left;
			node_l->left = nullptr;
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
			shared_ptr<Node<T>> node_l_ptr = this->left;
			this->left = nullptr;
			Node<T> *node_l = make_mut(&node_l_ptr);
			shared_ptr<Node<T>> node_l_r_ptr = node_l->right;
			node_l->right = nullptr;
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
			shared_ptr<Node<T>> node_r_ptr = this->right;
			this->right = nullptr;
			Node<T> *node_r = make_mut(&node_r_ptr);
			shared_ptr<Node<T>> node_r_l_ptr = node_r->left;
			node_r->left = nullptr;
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
			shared_ptr<Node<T>> node_r_ptr = this->right;
			this->right = nullptr;
			Node<T> *node_r = make_mut(&node_r_ptr);
			shared_ptr<Node<T>> node_r_r_ptr = node_r->right;
			node_r->right = nullptr;
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
};