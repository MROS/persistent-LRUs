#include "chain.h"
#include "config.h"
#include "workload.h"
#include "VC.h"
#include <string>
#include <cstdio>

using namespace std;

//extern VC global_vc;

int main(int argc, char **argv) {
	string f("config.toml");
	init_config(f);

//	global_vc = VC();

	Chain chain{};
	HardCodedWorkload workload = simple_workload();

	printf("開始工作\n");
	chain.work(workload);
}