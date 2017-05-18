#pragma once
#include "../header.h"

namespace tools {
	using namespace std;
	class SIMGenerator {
	private:
		vector<string> lines;
		unordered_set<string> types;
	public:
		void operator()(const std::string &);
		void genEnum(const std::string &);
		void genSimType(const std::string &);
		void genSim(const std::string &);
	};
	
	void customGen();
};

typedef tools::SIMGenerator coder;
using tools::customGen;