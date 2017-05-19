#include "builder.h"

base tools::baseObjBuilder::build(const std::string & name, GLuint va, GLuint shader, const base & other, int stride,
	const std::vector<interval> & intervals, const std::vector<std::string> & locs) const{	
	base ba = engine->createBaseObject(va, name);
	ba->setShaderID(shader);
	ba->shareMesh(other);
	ba->setSeperator(intervals);
	ba->setLocs(locs);
	ba->setStride(stride);
	ba->bind();
	return ba;
}

base tools::baseObjBuilder::build(const std::string & name, GLuint va, GLuint shader, const base & other, int stride,
	int verticeStart, int verticeCount,
	const std::vector<interval> & intervals, const std::vector<std::string> & locs) const{
	base ba = build(name, va, shader, other, stride, intervals, locs);
	ba->setVerticeStart(verticeStart);
	ba->setVerticeCount(verticeCount);
	return ba;
}

base tools::baseObjBuilder::build(const std::string & name, GLuint va, GLuint shader, const base & other, int stride,
	int verticeStart, int verticeCount, int elemStart, int elemEnd,
	const std::vector<interval> & intervals, const std::vector<std::string> & locs) const{
	base ba = build(name, va, shader, other, stride,verticeStart, verticeCount, intervals, locs);
	ba->setElementInterval(elemStart, elemEnd);
	return ba;
}

base tools::baseObjBuilder::build(const std::string & name, GLuint va, GLuint shader, const GLfloat * const mesh, int size, 
	int stride, const std::vector<interval> & intervals, const std::vector<std::string> & locs) const{
	base ba = engine->createBaseObject(va, name);
	ba->setShaderID(shader);
	ba->setMesh(mesh, size);
	ba->setVerticeStart(0);
	ba->setVerticeCount(size / stride);
	ba->setSeperator(intervals);
	ba->setLocs(locs);
	ba->setStride(stride);
	ba->bind();
	return ba;
}

base tools::baseObjBuilder::build(const std::string & name, GLuint va, GLuint shader, const GLfloat * const mesh, int size,
	int stride, int verticeStart, int verticeCount, const std::vector<interval> & intervals, const std::vector<std::string> & locs) const{
	base ba = build(name, va, shader, mesh, size, stride, intervals, locs);
	ba->setVerticeStart(verticeStart);
	ba->setVerticeCount(verticeCount);
	return ba;
}

base tools::baseObjBuilder::build(const std::string & name, GLuint va, GLuint shader, const GLfloat * const mesh, int meshSize,
	int stride, const GLshort * const element, int elemSize, const std::vector<interval> & intervals,
	const std::vector<std::string> & locs) const {
	base ba = build(name, va, shader, mesh, meshSize, stride, intervals, locs);
	ba->setElement(element, elemSize);
	ba->setElementInterval(0, elemSize);
	return ba;
}

base tools::baseObjBuilder::build(const std::string & name, GLuint va, GLuint shader, const GLfloat * const mesh, int meshSize,
	int stride, int verticeStart, int verticeCount, const GLshort * const element, int elemSize,
	const std::vector<interval> & intervals, const std::vector<std::string> & locs) const {
	base ba = build(name, va, shader, mesh, meshSize, stride, verticeStart, verticeCount, intervals, locs);
	ba->setElement(element, elemSize);
	ba->setElementInterval(0, elemSize);
	return ba;
}

base tools::baseObjBuilder::build(const std::string & name, GLuint va, GLuint shader, const GLfloat * const mesh, int meshSize,
	int stride, int verticeStart, int verticeCount, const GLshort * const element, int elemSize, int elemStart, int elemEnd,
	const std::vector<interval> & intervals, const std::vector<std::string> & locs) const {
	base ba = build(name, va, shader, mesh, meshSize, stride, verticeStart, verticeCount, element, elemSize, intervals, locs);
	ba->setElementInterval(elemStart, elemEnd);
	return ba;
}
