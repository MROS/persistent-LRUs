#include "rb_tree.hpp"
#include "lru.hpp"
#include <vector>
#include <iostream>
using namespace std;

int main() {
    IDPair<string, int> id_pair { "ggg", 9999 };
    auto root = make_shared<Node<IDPair<string, int>>>(Color::B, 123, id_pair);
    auto a = root->get_entry(123);
    if (a.has_value()) {
        cout << a->get()->value.id << endl;
    } else {
        cout << "找不到" << endl;
    }

    Node<IDPair<string, int>>::insert(&root, 23, id_pair, true);
    Node<IDPair<string, int>>::insert(&root, 3, id_pair, true);
    Node<IDPair<string, int>>::insert(&root, 1, id_pair, true);

    cout << root->debug() << endl;
    cout << root->left->debug() << endl;
    cout << root->right->debug() << endl;
    cout << root->left->left->debug() << endl;
}