#pragma once
#include <string>
#include <cpptoml.h>

struct Cache {
	std::string policy;
	std::string data_structure;
	int size;
};

struct Config {
	int port;
	Cache cache;
};

void init_config(std::string &config_file);
