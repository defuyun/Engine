#include "shader.h"

/*
	procedure for making a shader program:
	1. create shader program
	2. create shaders
	3. compile these shaders
	4. attach to the shader program
	5. link them together
	6. delete the shaders
*/

GLuint doge::shader::createProgram(const std::string & name) {
	if (map.find(name) != map.end()) {
		std::runtime_error("[SHA] creating program with name already exist: " + name + "\n");
	}
	GLuint pid = glCreateProgram();
	map[name] = pid;
	return pid;
}

GLuint doge::shader::createProgram(const std::string & name, const std::vector<std::pair<std::string, GLenum>> & files) {
	auto pid = this->createProgram(name);
	return this->add(pid, files);
}

GLuint doge::shader::getPid(const std::string & name) const {
	auto it = map.find(name);
	if (it == map.end()) {
		std::runtime_error("[SHA] Can't find shader program with name " + name + "\n");
	}
	return it->second;
}

GLuint doge::shader::add(GLuint pid, const std::pair<std::string,GLenum> & file) const {
	std::fstream fopen(file.first, std::fstream::in);
	if (!fopen.good()) {
		std::cerr << "[SHA] Can't open file " << file.first << "\n";
	}
	GLchar log[256];
	GLint success;

	std::stringstream buffer;
	buffer << fopen.rdbuf();
	// shaderID here is the actual single shader's id not the shader program id
	GLuint shaderID = glCreateShader(file.second);
	
	// buffer.str() creates a copy so we can't reference it or pass it into glShaderSource
	// so we need to create a new string using the move constructor so now there is a valid 
	// address when we do c_str()
	std::string str(buffer.str());
	const GLchar * cstr = str.c_str();
	glShaderSource(shaderID, 1, &cstr, 0);
	glCompileShader(shaderID);
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderID, sizeof(log), 0, log);
		std::cerr << "[SHA] failed to compile single shader " << file.first << ", error:\n"
			<< log << '\n';
	}

	glAttachShader(pid, shaderID);
	fopen.close();
	return shaderID;
}

GLuint doge::shader::add(GLuint pid,const std::vector<std::pair<std::string, GLenum>> & files) const {
	std::vector<GLuint> sids(files.size());
	for (size_t i = 0; i < files.size(); i++) {
		sids[i] = this->add(pid, files[i]);
	}
	glLinkProgram(pid);
	GLchar log[256];
	GLint success;
	glGetProgramiv(pid, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(pid, sizeof(log), 0, log);
		std::cerr << "[SHA] failed to link program " << pid << ", error:\n"
			<< log << '\n';
	}
	for (auto i : sids) {
		glDeleteShader(i);
	}
	return pid;
}

GLuint doge::shader::add(const std::string & name,
	const std::vector<std::pair<std::string, GLenum>> & files) const {
	return this->add(getPid(name), files);
}

GLuint doge::shader::add(const std::string & name,
	const std::pair<std::string, GLenum> & file) const {
	return this->add(getPid(name), file);
}