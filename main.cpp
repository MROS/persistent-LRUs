#include "chain.h"
#include "config.h"
#include "workload.h"
#include <string>
#include <cstdio>

using namespace std;

int main(int argc, char **argv) {
	string f("config.toml");
	init_config(f);

	Chain chain{};
	HardCodedWorkload workload = simple_workload();

	printf("開始工作\n");
	chain.work(workload);
}