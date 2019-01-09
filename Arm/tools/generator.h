#pragma once
#include "../header.h"

namespace tools {
	using namespace std;
	class SIMGenerator {
	public:
		vector<string> lines;
		unordered_set<string> enums;
		unordered_set<string> types;
		unordered_map<string, string> enu2type;
		unordered_map<string, string> type2func;
		unordered_map<string, string> type2var;
		unordered_map<string, vector<string>> func2param;
		void sim(const std::string &);
		void genEnum(const std::string &);
		void genSimType(const std::string &);
		void genSim(const std::string &);
	};
	
	void customGen();
};

typedef tools::SIMGenerator generator;
using tools::customGen;