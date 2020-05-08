#include "node.hpp"
#include "rb_tree.hpp"
#include <iostream>
using namespace std;

int main() {
    try{
        string str = "ggg";
        auto root = make_shared<Node<string>>(Color::B, 123, str);
        auto a = root->get_entry(123);
        if (a != nullptr) {
            cout << a->value << endl;
        } else {
            cout << "找不到" << endl;
        }

        auto on_clone = [](const Node<string>& node) {
            cout << "clone!! " + node.debug() << endl;
        };

        Node<string>::insert(&root, 23, str, true, on_clone);
        Node<string>::insert(&root, 3, str, true, on_clone);
        Node<string>::insert(&root, 1, str, true, on_clone);

        cout << root->debug() << endl;
        cout << root->left->debug() << endl;
        cout << root->right->debug() << endl;
        cout << root->left->left->debug() << endl;

        auto root2 = root->clone();
        Node<string>::insert(&root2, 99, str, true, on_clone);
        cout << root2->right->left->debug() << endl;
        if (root->right->left == nullptr) {
            cout << "原樹沒有 99 節點" << endl;
        }

        Node<string>::remove(&root2, 3, true, on_clone);
        cout << root2->debug() << endl;
        cout << root2->left->debug() << endl;
        cout << root2->right->debug() << endl;
        cout << root2->right->left->debug() << endl;
    } catch (const char * str) {
        std::cout << "Exception: " << str << std::endl;
    }
}