#include "order_tree.hpp"
#include "lru.hpp"
#include <vector>
#include <memory>
using namespace std;

int main() {
//	auto lru = OrderTreeLRU<int, int>(16);

    auto order_tree =  make_shared<OrderTree<int, int>>(3);
    order_tree->show();
    vector<OrderTreeNode<int, int>*> nodes;
    for (int i = 0; i < 4; i++) {
        auto node = order_tree->add(i, i);
        nodes.push_back(node);
        order_tree->show();
		order_tree->show_order();
    }
//  order_tree->change_node(nodes[0], 100, 100);
//	order_tree->show();
//	order_tree->show_order();
//	for (int i = 0; i < 3; i++) {
//		order_tree->to_head(nodes[i]);
//		order_tree->show();
//		order_tree->show_order();
//	}
    cout << "測試 put" << endl;
    // 測試 put
    auto br1 = order_tree->new_version();
	auto put_ret = br1->put(nodes[0], nodes[0]->key, 100);
	auto front = put_ret.new_node;
	br1->show();

	put_ret = br1->put(front, front->key, 101);
	front = put_ret.new_node;
	br1->show();

	put_ret = br1->put(front, front->key, 102);
	front = put_ret.new_node;
	br1->show();

	put_ret = br1->put(front, front->key, 103);
	br1->show();
	front = put_ret.reorder->operator[](3);

	put_ret = br1->put(front, front->key, 104);
	front = put_ret.new_node;
	br1->show();
//
//	cout << "測試 get" << endl;
//	// 測試 get
//    auto ret = order_tree->get(nodes[0]);
//    order_tree = ret.new_tree;
//    order_tree->show();
//	order_tree->show_order();
//
//    ret = order_tree->get(nodes[1]);
//    order_tree = ret.new_tree;
//    order_tree->show();
//	order_tree->show_order();
//
//    ret = order_tree->get(nodes[2]);
//    order_tree = ret.new_tree;
//    order_tree->show();
//	order_tree->show_order();
//
//    ret = order_tree->get(nodes[3]);
//    order_tree = ret.new_tree;
//    order_tree->show();
//	order_tree->show_order();
//
//	ret = order_tree->get(nodes[0]);
//	order_tree = ret.new_tree;
//	order_tree->show();
//	order_tree->show_order();
}