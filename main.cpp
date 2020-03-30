#include "node.h"
#include <cereal/archives/binary.hpp>
#include <iostream>
#include <fstream>
#include "transaction.h"
#include "block.h"

using namespace std;

int main(int argc, char **argv) {
	string config("config.toml");
	Node node(config);
	node.start();
}