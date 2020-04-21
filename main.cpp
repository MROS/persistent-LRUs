#include "node.h"
#include "config.h"
#include <string>

using namespace std;

int main(int argc, char **argv) {
	string f("config.toml");
	init_config(f);
	Node node{};
	node.start();
}