#include "rb_tree.hpp"
#include "lru.hpp"
#include <vector>
#include <iostream>
using namespace std;

int main() {
    IDPair<string, int> id_pair { "ggg", 9999 };
    Node<IDPair<string, int>> node(Color::B, 123, id_pair);
    auto a = node.get_entry(123);
    if (a.has_value()) {
        cout << a->get()->key << endl;
    } else {
        cout << "找不到" << endl;
    }
    node.balance();
}