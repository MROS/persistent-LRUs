#include <iostream>
#include "heap.hpp"

int main() {
	Heap<int> heap(100);
	heap.add(100);
	heap.show();
	for (int i = 0; i < 10; i++) {
		heap.add(i);
	}
	heap.show();
}