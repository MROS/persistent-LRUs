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

string DIR = "test_case/immutable/";

vector<string> cases = {
	"1",
//	"2",
//	"3",
//	"4",
//	"5"
};

using Cmd = std::variant<Get<int>, Put<int, int>>;

struct BatchOp {
	vector<Cmd> cmds;
	int id;
};
struct ReadOnlyOp {
	vector<int> keys;
	int id;
};

struct Input {
	size_t capacity;
	vector<variant<BatchOp, ReadOnlyOp>> ops;
};

using Ans = vector<vector<optional<int>>>;

Ans read_ans(string &name) {
	string dir = DIR;
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
	string dir = DIR;
	Input input;
	string path = dir + name + ".in";
	ifstream file;
	file.open(path, ios::in);
	if (!file) {
		cout << "無法讀取 " << path << endl;
	}
	file >> input.capacity;
//	cout << "capacity: " << input.capacity << endl;

	// 讀取 in
	while (true) {
		string op, from;
		int n;
		file >> op;
		if (op == "batch") {
			BatchOp op;
			file >> n >> from >> op.id;
//			cerr << "batch " << n << " from " << op.id << endl;
			vector<Cmd> &cmds = op.cmds;
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
			input.ops.emplace_back(op);
		} else if (op == "read") {
			ReadOnlyOp op;
			file >> n >> from >> op.id;
//			cerr << "read " << n << " from " << op.id << endl;
//			cout << "read " << n << endl;
			for (int i = 0; i < n; i++) {
				int k;
				file >> k;
				op.keys.push_back(k);
			}
			input.ops.emplace_back(op);
		} else {
			break;
		}
	}
	file.close();
//	cout << "唯獨組數: " << input.ans.size() << endl;
	return input;
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

		vector<shared_ptr<LRU<int, int>>> versions = { lru_base.create(tc.capacity) };

		int b = 0;
		int r = 0;
		for (auto op: tc.ops) {
			if (get_if<BatchOp>(&op) != nullptr) {
				cout << "第 " << b << " 組批量操作" << endl;
				auto batch_op = get<BatchOp>(op);
				int id = batch_op.id;
				versions.push_back(versions[id]->batch_operate(batch_op.cmds));
				b++;
			} else {
				cerr << "第 " << r << " 組" << "唯讀" << endl;
				auto rd_op = get<ReadOnlyOp>(op);
				int id = rd_op.id;
				for (int i = 0; i < rd_op.keys.size(); i++) {
					auto ret = versions[id]->read_only_get(rd_op.keys[i]);
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
//	cerr << "共有 " << tc.ops.size() << " 組操作" << endl;

	vector<shared_ptr<LRU<int, int>>> versions = { lru_base.create(tc.capacity) };

	cerr << "輸出 " << name << " 開始" << endl;
	int b = 0;
	int r = 0;
	for (auto op: tc.ops) {
		if (get_if<BatchOp>(&op) != nullptr) {
			cerr << "第 " << b << " 組批量操作" << endl;
			auto batch_op = get<BatchOp>(op);
			int id = batch_op.id;
			versions.push_back(versions[id]->batch_operate(batch_op.cmds));
			b++;
		} else {
			cerr << "第 " << r << " 組" << "唯讀" << endl;
			auto rd_op = get<ReadOnlyOp>(op);
			int id = rd_op.id;
			auto keys = rd_op.keys;
			for (int i = 0; i < keys.size(); i++) {
				auto ret = versions[id]->read_only_get(keys[i]);
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
	SimpleCopyLRU<int, int> simple_copy_lru;
	test(simple_copy_lru);
//	OrderTreeLRU<int, int> order_tree_lru;
//	test(order_tree_lru);
//	RBTreeLRU<int, int> rb_tree_lru;
//	test(rb_tree_lru);
}