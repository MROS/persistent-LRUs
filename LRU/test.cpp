#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <variant>
#include "simple-copy/lru.hpp"
#include "rb-tree/lru.hpp"
#include "order-tree/lru.hpp"
#include "LRU.h"
using namespace std;

vector<string> cases = {
//	"1",
//	"2",
//	"3",
//	"4",
	"5"
};

using Cmd = std::variant<Get<int>, Put<int, int>>;

using BatchOp = std::vector<Cmd>;
using ReadOnlyOp = std::vector<int>;

struct Input {
	size_t capacity;
	vector<variant<BatchOp, ReadOnlyOp>> ops;
};

using Ans = vector<vector<optional<int>>>;

Ans read_ans(string &name) {
	string dir = "test_case/";
	string path = dir + name + ".out";
	ifstream file;
	file.open(path, ios::in);
	if (!file) {
		cout << "無法讀取 " << path << endl;
	}
	Ans ret;
	vector<optional<int>> ans;
	while (!file.eof()) {
		ans = {};
		string x;
		getline(file, x);
		if (x.length() == 0) {
			continue;
		} else if (x[0] == 'n') {
			ans.emplace_back(nullopt);
		} else {
			ans.emplace_back(stoi(x));
		}
		while (true) {
			getline(file, x);
			if (x.length() == 0) {
				break;
			} else if (x[0] == 'n') {
				ans.emplace_back(nullopt);
			} else {
				ans.emplace_back(stoi(x));
			}
		}
		ret.push_back(ans);
	}
	file.close();
	return ret;
}

Input read_input(string &name) {
	string dir = "test_case/";
	Input ret;
	string path = dir + name + ".in";
	ifstream file;
	file.open(path, ios::in);
	if (!file) {
		cout << "無法讀取 " << path << endl;
	}
	file >> ret.capacity;
//	cout << "capacity: " << ret.capacity << endl;

	// 讀取 in
	while (true) {
		string op;
		file >> op;
		vector<Cmd> cmds;
		if (op == "batch") {
			int n;
			file >> n;
//			cout << "batch " << n << endl;
			for (int i = 0; i < n; i++) {
				string cmd;
				file >> cmd;
				if (cmd == "get") {
					Get<int> get{};
					file >> get.key;
					cmds.emplace_back(get);
				} else {
					Put<int, int> put{};
					file >> put.key >> put.value;
					cmds.emplace_back(put);
				}
			}
			ret.ops.emplace_back(cmds);
		} else if (op == "read") {
			int n;
			file >> n;
//			cout << "read " << n << endl;
			vector<int> rs;
			for (int i = 0; i < n; i++) {
				int k;
				file >> k;
				rs.push_back(k);
			}
			ret.ops.emplace_back(rs);
		} else {
			break;
		}
	}
	file.close();
//	cout << "唯獨組數: " << ret.ans.size() << endl;
	return ret;
}

string to_s(optional<int> x) {
	if (x == nullopt) {
		return "null";
	} else {
		return to_string(*x);
	}
}

void test(LRU<int, int> &lru_base) {
	for (auto name : cases) {

		cout << "測試 " << name << " 開始" << endl;
		auto tc = read_input(name);
		auto ans = read_ans(name);

		auto lru = lru_base.create(tc.capacity);

		int b = 0;
		int r = 0;
		for (auto op: tc.ops) {
			if (get_if<BatchOp>(&op) != nullptr) {
				cout << "第 " << b << " 組批量操作" << endl;
				auto batch_op = get<BatchOp>(op);
				lru = lru->batch_operate(batch_op);
				b++;
			} else {
				cerr << "第 " << r << " 組" << "唯讀" << endl;
				auto keys = get<ReadOnlyOp>(op);
				for (int i = 0; i < keys.size(); i++) {
					auto ret = lru->read_only_get(keys[i]);
					if (ret != ans[r][i]) {
						cerr << "第 " << r << " 組" << "唯讀的第 " << i << " 項" << ": ";
						cerr << "正解: " << to_s(ans[r][i]) << ", 輸出: " << to_s(ret) << endl;
					}
				}
				r++;
			}
		}
		cout << "測試 " << name << " 結束" << endl;
	}
}

void print_ans(LRU<int, int> &lru_base, string name) {

	auto tc = read_input(name);

	auto lru = lru_base.create(tc.capacity);

	cerr << "輸出 " << name << " 開始" << endl;
	int b = 0;
	int r = 0;
	for (auto op: tc.ops) {
		if (get_if<BatchOp>(&op) != nullptr) {
			cerr << "第 " << b << " 組批量操作" << endl;
			auto batch_op = get<BatchOp>(op);
			lru = lru->batch_operate(batch_op);
			b++;
		} else {
			cerr << "第 " << r << " 組" << "唯讀" << endl;
			auto keys = get<ReadOnlyOp>(op);
			for (int i = 0; i < keys.size(); i++) {
				auto ret = lru->read_only_get(keys[i]);
				if (ret == nullopt) {
					cout << "null" << endl;
				} else {
					cout << *ret << endl;
				}
			}
			cout << endl;
			r++;
		}
	}
	cerr << "輸出 " << name << " 結束" << endl;
}
int main(int argc, char *argv[]) {
//	SimpleCopyLRU<int, int> simple_copy_lru;
//	test(simple_copy_lru);
//	print_ans(simple_copy_lru, string("1"));
//	print_ans(simple_copy_lru, string("2"));
//	print_ans(simple_copy_lru, string("3"));
//	print_ans(simple_copy_lru, string("4"));
//	print_ans(simple_copy_lru, string("5"));
//	SimpleCopyLRU<int, int> simple_copy_lru;
//	test(simple_copy_lru);
//	OrderTreeLRU<int, int> order_tree_lru;
//	test(order_tree_lru);
	RBTreeLRU<int, int> rb_tree_lru;
	test(rb_tree_lru);
}