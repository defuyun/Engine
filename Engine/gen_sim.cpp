#include "gen_sim.h"
#include <fstream>
#include <sstream>

std::string toup(const std::string & str) {
	std::string cpy = str;
	for (auto & c : cpy) {
		c = toupper(c);
	}
	return cpy;
}

std::string joinEnum(std::unordered_set<std::string> & types) {
	std::string str = "enum type : unsigned int {\n\t";
	for (auto & s : types) {
		str += s + ",";
	}
	str += "\n};\n";
	return str;
}

std::string joinVar(const std::string & type, const std::string & varname) {
	return "std::unordered_map<std::string,std::unordered_map<std::string," + type + ">> " + varname + ";\n";
}

std::string joinGetStorage(const std::string & type, const std::string & varname) {
	return "template<> const std::unordered_map<std::string, std::unordered_map<std::string," + type + ">>"
		+ " & getStorage<" + type + ">() const{\n" +
		"\treturn " + varname + ";\n" +
		"}\n";
}

void replace(std::string & str, char from, char to) {
	for (auto & c : str) {
		if (c == from) 
			c = to;
	}
}

void tools::SIMGenerator::operator()(const std::string & infile) {
	using namespace std;
	fstream in(infile, std::fstream::in);

	std::string line;
	while (std::getline(in, line)) {
		replace(line, '$', 't');
		lines.emplace_back(line);
		std::stringstream ss(line);
		string enu;
		ss >> enu;
		types.emplace(enu);
	}
	in.close();
}

void tools::SIMGenerator::genEnum(const std::string & outfile) {
	fstream out(outfile, std::fstream::out);
	out << joinEnum(this->types);
	out.close();
}

void tools::SIMGenerator::genSimType(const std::string & outfile) {
	fstream out(outfile, std::fstream::out);
	for (auto & p : lines) {
		std::stringstream ss(p);
		string enu, type, varname;
		ss >> enu >> type >> varname;
		out << joinVar(type, varname);
	}
	out.close();
}

void tools::SIMGenerator::genSim(const std::string & outfile) {
	fstream out(outfile, std::fstream::out);
	for (auto & p : lines) {
		std::stringstream ss(p);
		string enu, type, varname;
		ss >> enu >> type >> varname;
		out << joinGetStorage(type, varname);
	}
	std::string fundec = "void use(GLuint shaderID, const shaderIndexWrapper & si) const {\n";
	bool first = true;
	for (auto & p : lines) {
		std::stringstream ss(p);
		string enu, type, varname, funcname;
		ss >> enu >> type >> varname >> funcname;
		string ifs = "\t";
		if (first) {
			ifs += "if ";
			first = false;
		}
		else {
			ifs += "else if ";
		}

		ifs += "(si.data == " + enu + " ) {\n\t\t" +
			type + " t = this->get<" + type + ">(si);\n\t\t" +
			funcname + "(glGetUniformLocation(shaderID,si.loc.c_str())";
		string param;
		while (ss >> param) {
			ifs += ", " + param;
		}
		ifs += ");\n\t}\n";
		fundec += ifs;
	}
	fundec += "}\n";
	out << fundec;
	out.close();
}

void tools::customGen() {	
	coder gen;
	gen("simtypes.txt");
	gen.genEnum("enum.temp");
	gen.genSim("simauto.temp");
	gen.genSimType("simtype.temp");
}