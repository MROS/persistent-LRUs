#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <variant>
#include "simple-copy/lru.hpp"
#include "LRU.h"
using namespace std;

vector<string> cases = {
	"1"
};

using Cmd = std::variant<Get<int>, Put<int, int>>;

using BatchOp = std::vector<Cmd>;
using ReadOblyOp = std::vector<int>;

struct TestCase {
	size_t capacity;
	vector<variant<BatchOp, ReadOblyOp>> ops;
	vector<vector<optional<int>>> ans;
};

TestCase test_case(string &name) {
	string dir = "test_case/";
	TestCase ret;
	string path = dir + name + ".in";
	ifstream file;
	file.open(path, ios::in);
	if (!file) {
		cout << "無法讀取 " << path << endl;
	}
	file >> ret.capacity;
//	cout << "capacity: " << ret.capacity << endl;

	vector<int> rd_only_len;
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
			rd_only_len.push_back(n);
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

	// 讀取 out
	path = dir + name + ".out";
	file.open(path, ios::in);
	if (!file) {
		cout << "無法讀取 " << path << endl;
	}
	for (auto len: rd_only_len) {
		vector<optional<int>> ans;
		for (int i = 0; i < len; i++) {
			string x;
			file >> x;
			if (x[0] == 'n') {
				ans.emplace_back(nullopt);
			} else {
				ans.emplace_back(stoi(x));
			}
		}
		ret.ans.push_back(ans);
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
		auto tc = test_case(name);

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
				auto keys = get<ReadOblyOp>(op);
				for (int i = 0; i < keys.size(); i++) {
					auto ret = lru->read_only_get(keys[i]);
					if (ret != tc.ans[r][i]) {
						cerr << "第 " << r << " 組" << "唯讀的第 " << i << " 項" << ": ";
						cerr << "正解: " << to_s(tc.ans[r][i]) << ", 輸出: " << to_s(ret) << endl;
					} else {
						cerr << "第 " << r << " 組" << "唯讀的第 " << i << " 項" << ": ";
						cerr << "正解: " << to_s(tc.ans[r][i]) << ", 輸出: " << to_s(ret) << endl;
					}
				}
				r++;
			}
		}
		cout << "測試 " << name << " 結束" << endl;
	}
}

int main(int argc, char *argv[]) {
	SimpleCopyLRU<int, int> simple_copy_lru;
	test(simple_copy_lru);
}