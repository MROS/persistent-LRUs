#include "node.h"
#include <cereal/archives/binary.hpp>
#include <iostream>
#include <fstream>
#include "transaction.h"
#include "block.h"

using namespace std;

int main(int argc, char **argv) {
	std::ofstream os("out.cereal", std::ios::binary);
	cereal::BinaryOutputArchive archive( os );

	Transaction tx1 { 1, 2, 100 };
	Transaction tx2 { 2, 3, 70 };

	Block block { 7, { tx1, tx2 } };
	archive(block);

	string config("config.toml");
	Node node(config);
	node.start();
}