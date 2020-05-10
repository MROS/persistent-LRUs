#include "node.hpp"
#include "rb_tree.hpp"
#include "lru.hpp"
#include <iostream>
#include <vector>
using namespace std;

typedef std::variant<Get<string>, Put<string, float>> Cmd;

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

        cout << "移除最小" << endl;
        rbtree.remove_least();
        n = rbtree.find_by_id("aaa");
        cout << n << endl;

        cout << "LRU" << endl;
        shared_ptr<LRU<string, float>> lru = make_shared<RBTreeLRU<string, float>>(2);
        vector<Cmd> cmds{Put<string, float>{ "bbb", 3.0 }, Put<string, float>{ "ccc", 4.0 }, Put<string, float>{ "ttt", 5.0 }};
        auto next_lru = lru->batch_operate(cmds);

        string key {"ccc"};
        auto t = next_lru->read_only_get(key);
        cout << t.value() << endl;

        key = "ttt";
        t = next_lru->read_only_get(key);
        cout << t.value() << endl;

        key = "bbb";
        t = next_lru->read_only_get(key);
        cout << (t.has_value() ? "有值" : "無值") << endl;
    } catch (const char * str) {
        std::cout << "Exception: " << str << std::endl;
    }
}