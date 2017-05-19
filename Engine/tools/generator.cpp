#include "generator.h"

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
	return "template<> std::unordered_map<std::string, std::unordered_map<std::string," + type + ">>"
		+ " & getStorage<" + type + ">() {\n" +
		"\treturn " + varname + ";\n" +
		"}\n";
}

std::string joinConstGetStorage(const std::string & type, const std::string & varname) {
	return "template<> const std::unordered_map<std::string, std::unordered_map<std::string," + type + ">>"
		+ " & getStorage<" + type + ">() const {\n" +
		"\treturn " + varname + ";\n" +
		"}\n";
}

void replace(std::string & str, char from, char to) {
	for (auto & c : str) {
		if (c == from) 
			c = to;
	}
}

void tools::SIMGenerator::sim(const std::string & infile) {
	using namespace std;
	fstream in(infile, std::fstream::in);

	std::string line;
	while (std::getline(in, line)) {
		replace(line, '$', 't');
		lines.emplace_back(line);
		std::stringstream ss(line);
		string enu, type;
		ss >> enu;
		ss >> type;
		enums.emplace(enu);
		enu2type.emplace(enu, type);
		if (!types.count(type)) {
			types.emplace(type);
			string varname, funcname, param;
			ss >> varname;
			type2var.emplace(type, varname);
			ss >> funcname;
			type2func.emplace(type, funcname);
			while (ss >> param) {
				func2param[funcname].emplace_back(param);
			}
		}
	}
	in.close();
}

void tools::SIMGenerator::genEnum(const std::string & outfile) {
	fstream out(outfile, std::fstream::out);
	out << joinEnum(this->enums);
	out.close();
}

void tools::SIMGenerator::genSimType(const std::string & outfile) {
	fstream out(outfile, std::fstream::out);
	for (auto & p : types) {
		out << joinVar(p, type2var[p]);
	}
	out.close();
}

void tools::SIMGenerator::genSim(const std::string & outfile) {
	fstream out(outfile, std::fstream::out);
	for (auto & p : types) {
		out << joinGetStorage(p, type2var[p]);
		out << joinConstGetStorage(p, type2var[p]);
	}
	std::string fundec = "void use(GLuint shaderID, const shaderIndexWrapper & si) const {\n";
	bool first = true;
	for (auto & p : enums) {
		string enu = p;
		string type = enu2type[p];
		string funcname = type2func[type];
		vector<string> params = func2param[funcname];
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
		for (auto & param : params) {
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
	generator gen;
	gen.sim("simtypes.txt");
	gen.genEnum("doge/sim/enum.temp");
	gen.genSim("doge/sim/simauto.temp");
	gen.genSimType("doge/sim/simtype.temp");
}