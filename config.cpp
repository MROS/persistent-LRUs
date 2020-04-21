#include "config.h"
#include <string>
#include <cstdlib>
#include <iostream>

using namespace std;

Config config;

void init_config(std::string &config_file) {
	auto toml = cpptoml::parse_file(config_file);

	auto cache_policy = toml->get_qualified_as<string>("cache.policy");
	if (cache_policy) {
		config.cache.policy = *cache_policy;
		cout << "快取策略 = " << *cache_policy << endl;
	} else {
		cerr << "無快取策略" << endl;
		exit(1);
	}

	auto data_structure = toml->get_qualified_as<string>("cache.data-structure");
	if (data_structure) {
		config.cache.data_structure = *data_structure;
		cout << "資料結構 = " << *data_structure << endl;
	} else {
		cerr << "未指定資料結構" << endl;
		exit(1);
	}

	auto cache_size = toml->get_qualified_as<int>("cache.size");
	if (cache_size) {
		config.cache.size = *cache_size;
		cout << "快取大小 = " << *cache_size << endl;
	} else {
		cerr << "未指定快取大小" << endl;
		exit(1);
	}

	auto port = toml->get_qualified_as<int>("network.port").value_or(0);
	if (port) {
		config.port = port;
		cout << "port = " << config.port << endl;
	} else {
		cerr << "未指定埠口" << endl;
		exit(1);
	}

}
