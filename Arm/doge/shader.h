#pragma once
#include "../header.h"

namespace doge {
	class shader {
	private:
		std::unordered_map<std::string, GLuint> map;
	public:
		// creates a shader progam, just an empty skeleton with no source files
		// @return program id
		GLuint createProgram(const std::string & name);

		// creates a shader program, compile the files in shader and link them
		// @return program id
		GLuint createProgram(const std::string & name, const std::vector<std::pair<std::string, GLenum>> & files);
		
		// add files to the empty shader program
		// this function does not clean up the shader created so you need to call glDeleteShader on
		// the return value
		// also doesn't check if pid is valid so advised to use the name add instead
		// @return the shader id of that file
		GLuint add(GLuint pid, const std::pair<std::string, GLenum> & file) const;
	
		// adds multiple files, assumes that all the file make up the shader program
		// so it will attempt to compile afterwards
		// need at least 1 fragment shader and 1 vertex shader in the vect
		// @return program id
		GLuint add(GLuint pid, const std::vector<std::pair<std::string, GLenum>> & files) const;
		
		// name function of add, recommend using this function instead of pid
		// @returns shader id
		GLuint add(const std::string & name, const std::pair<std::string, GLenum> & file) const;
		
		// name function of add, recommend using this function instead of pid
		// @return program id
		GLuint add(const std::string & name, const std::vector<std::pair<std::string, GLenum>> & files) const;
		
		GLuint getPid(const std::string &) const;
	};

	typedef std::pair<std::string, GLenum> shfile;
};
