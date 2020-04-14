#include "node.h"
#include "VC.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
	string config("config.toml");
	Node node(config);
	node.start();
}