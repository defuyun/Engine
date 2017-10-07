#include "generator.h"

std::string joinEnum(std::unordered_set<std::string> & types) {
	std::string str = "enum type : unsigned int {\n\t";
	for (auto & s : types) {
		str += s + ",";
	}
	str += "\n};\n";
	return str;
}

std::string joinVar(const std::string & type, const std::string & varname) {
	return "std::unordered_map<shaderIndexWrapper, std::shared_ptr<" + type + ">> " + varname + ";\n";
}

std::string joinGetStorage(const std::string & type, const std::string & varname) {
	return "template<> std::unordered_map<shaderIndexWrapper, std::shared_ptr<" + type + ">>"
		+ " & getStorage<" + type + ">() {\n" +
		"\treturn " + varname + ";\n" +
		"}\n";
}

std::string joinConstGetStorage(const std::string & type, const std::string & varname) {
	return "template<> const std::unordered_map<shaderIndexWrapper,std::shared_ptr<" + type + ">>"
		+ " & getStorage<" + type + ">() const {\n" +
		"\treturn " + varname + ";\n" +
		"}\n";
}

std::string genUse(const tools::SIMGenerator & generator)
{
	std::string fundec = "void use(GLuint shaderID, const shaderIndexWrapper & si) {\n";
	bool first = true;
	for (auto & p : generator.enums) {
		std::string enu = p;
		std::string type = generator.enu2type.find(p)->second;
		std::string funcname = generator.type2func.find(type)->second;
		std::vector<std::string> params = generator.func2param.find(funcname)->second;
		std::string ifs = "\t";
		if (first) {
			ifs += "if ";
			first = false;
		}
		else {
			ifs += "else if ";
		}

		ifs += "(si.type_ == " + enu + " ) {\n\t\t" +
			type + "& t = *(this->get<" + type + ">(si).lock());\n\t\t" +
			funcname + "(glGetUniformLocation(shaderID,si.loc_.c_str())";
		for (auto & param : params) {
			ifs += ", " + param;	
		}
		ifs += ");\n\t}\n";
		fundec += ifs;
	}
	fundec += "}\n";
	return fundec;
}

std::string genGetData(const tools::SIMGenerator & generator)
{
	std::string fundec = "void addData(const shaderIndexWrapper & si, shaderDataWrapperT & data) {\n";
	bool first = true;
	for (auto & p : generator.enums) {
		std::string enu = p;
		std::string type = generator.enu2type.find(p)->second;
		std::string ifs = "\t";
		if (first) {
			ifs += "if ";
			first = false;
		}
		else {
			ifs += "else if ";
		}

		ifs += "(si.type_ == " + enu + " ) {\n\t\t" +
			"add(converter::get<" + type + ">(data.get()));\n";
		ifs += "\t}\n";
		fundec += ifs;
	}
	fundec += "}\n";
	return fundec;
}

std::string genSanityCheck(const tools::SIMGenerator & generator)
{
	std::string fundec = "bool sanityCheck(const shaderIndexWrapper & si) {\n";
	bool first = true;
	for (auto & p : generator.enums) {
		std::string enu = p;
		std::string type = generator.enu2type.find(p)->second;
		std::string ifs = "\t";
		if (first) {
			ifs += "if ";
			first = false;
		}
		else {
			ifs += "else if ";
		}

		ifs += "(si.type_ == " + enu + " ) {\n\t\t" +
			"return getStorage<" + type + ">().find(si) != getStorage<"
			+ type + ">().end();";
		ifs += "\n\t}\n";
		fundec += ifs;
	}
	fundec += "\treturn false\n}\n";
	return fundec;
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
	}
	out << genUse(*this);
	out << genSanityCheck(*this);
	out.close();
}

void tools::customGen() {	
	generator gen;
	gen.sim("simtypes.txt");
	gen.genEnum("doge/sim/enum.temp");
	gen.genSim("doge/sim/simauto.temp");
	gen.genSimType("doge/sim/simtype.temp");
}