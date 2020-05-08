#include "node.hpp"
#include "rb_tree.hpp"
#include "lru.hpp"
#include <iostream>
using namespace std;

void debug_pair(const IDPair<string, float> &pair) {
    cout << "ID: " << pair.id << ", Value: " << pair.value << endl;
}

int main() {
    try{
        string str = "ggg";
        auto root = make_shared<Node<string>>(Color::B, 123, str);
        auto a = Node<string>::get_node(root, 123);
        if (a != nullptr) {
            cout << a->get()->entry->value << endl;
        } else {
            cout << "找不到" << endl;
        }

        Node<string>::insert(&root, 23, str, true);
        Node<string>::insert(&root, 3, str, true);
        Node<string>::insert(&root, 1, str, true);

        cout << root->debug() << endl;
        cout << root->left->debug() << endl;
        cout << root->right->debug() << endl;
        cout << root->left->left->debug() << endl;

        auto root2 = root->clone();
        Node<string>::insert(&root2, 99, str, true);
        cout << root2->right->left->debug() << endl;
        if (root->right->left == nullptr) {
            cout << "原樹沒有 99 節點" << endl;
        }

        Node<string>::remove(&root2, 3, true);
        cout << root2->debug() << endl;
        cout << root2->left->debug() << endl;
        cout << root2->right->debug() << endl;
        cout << root2->right->left->debug() << endl;

        RedBlackTree<string, float> rbtree;
        rbtree.insert(3, "aaa", 0.233);
        auto n = rbtree.find_by_id("aaa");
        debug_pair(n->get()->entry->value);

        rbtree.remove_least();
        n = rbtree.find_by_id("aaa");
        cout << n << endl;

        rbtree.remove_least();

        // RBTreeLRU<string, float> lru(100);
        // string key{"aaa"};
        // lru.read_only_get(key);
    } catch (const char * str) {
        std::cout << "Exception: " << str << std::endl;
    }
}