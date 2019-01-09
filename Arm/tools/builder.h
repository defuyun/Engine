#pragma once
#include "../doge/engine.h"

namespace tools{
	// I may have the builder contain some states in the future
	class baseObjBuilder {
	public:
		base build(const std::string & name, GLuint va, GLuint shader, const base & other, int stride,
			const std::vector<interval> & intervals, const std::vector<std::string> & locs) const;
		
		base build(const std::string & name, GLuint va, GLuint shader, const base & other, int stride,
			int verticeStart, int verticeCount,
			const std::vector<interval> & intervals, const std::vector<std::string> & locs) const;
		
		base build(const std::string & name, GLuint va, GLuint shader, const base & other, int stride,
			int verticeStart, int verticeCount, int elemStart, int elemEnd,
			const std::vector<interval> & intervals, const std::vector<std::string> & locs) const;
		
		base build(const std::string & name, GLuint va, GLuint shader, const GLfloat * const mesh, int size, int stride,
			const std::vector<interval> & intervals, const std::vector<std::string> & locs) const ;

		base build(const std::string & name, GLuint va, GLuint shader, const GLfloat * const mesh, int size, int stride,
			int verticeStart, int verticeCount, const std::vector<interval> & intervals, const std::vector<std::string> & locs) const;
		
		base build(const std::string & name, GLuint va, GLuint shader, const GLfloat * const mesh, int meshSize, int stride,
			const GLshort * const element, int elemSize, const std::vector<interval> & intervals, 
			const std::vector<std::string> & locs) const;
		
		base build(const std::string & name, GLuint va, GLuint shader, const GLfloat * const mesh, int meshSize, int stride,
			int verticeStart, int verticeCount, const GLshort * const element, int elemSize, 
			const std::vector<interval> & intervals, const std::vector<std::string> & locs) const;
		
		base build(const std::string & name, GLuint va, GLuint shader, const GLfloat * const mesh, int meshSize, int stride,
			int verticeStart, int verticeCount, const GLshort * const element, int elemSize, int elemStart, int elemEnd,
			const std::vector<interval> & intervals, const std::vector<std::string> & locs) const;
	};

	extern std::unique_ptr<baseObjBuilder> builder;
};

using tools::builder;
