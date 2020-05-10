#include <iostream>
#include "simple-copy/lru.hpp"
using namespace std;

int main() {
	cout << "start test" << endl;
	SimpleCopyLRU<int, int> simple_copy_lru(10);
}