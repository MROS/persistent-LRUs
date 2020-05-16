#include "order_tree.hpp"
#include "lru.hpp"
#include <vector>
#include <memory>
using namespace std;

int main() {
//	auto lru = OrderTreeLRU<int, int>(16);

    auto order_tree =  make_shared<OrderTree<int, int>>(3);
    order_tree->show();
    vector<shared_ptr<DoublyLinkedNode<int>>> nodes;
    for (int i = 0; i < 4; i++) {
        auto ret = order_tree->add(i);
        order_tree = ret.first;
        nodes.push_back(ret.second);
        order_tree->show();
    }
//	auto put_ret = order_tree->change_value(nodes[0], 100);
//	put_ret.first->show();
	auto put_ret = order_tree->put(nodes[0], 100);
	put_ret.first->show();

    auto ret = order_tree->get(nodes[0]);
    order_tree = ret.new_tree;
    order_tree->show();
	order_tree->show_order();

    ret = order_tree->get(nodes[1]);
    order_tree = ret.new_tree;
    order_tree->show();
	order_tree->show_order();

    ret = order_tree->get(nodes[2]);
    order_tree = ret.new_tree;
    order_tree->show();
	order_tree->show_order();

    ret = order_tree->get(nodes[3]);
    order_tree = ret.new_tree;
    order_tree->show();
	order_tree->show_order();

	ret = order_tree->get(nodes[0]);
	order_tree = ret.new_tree;
	order_tree->show();
	order_tree->show_order();
}