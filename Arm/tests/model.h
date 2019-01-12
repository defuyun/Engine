#pragma once

#include <GL/glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <vector>

enum TextureType {
	diffuse,
	specular
};

struct Texture {
	GLuint id;
	TextureType type;
};

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

class Mesh {
public:
	GLuint vao, vbo, ebo;
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<GLuint> indices;
	Mesh(const std::vector<Vertex> & vertices, const std::vector<Texture> & textures, const std::vector<GLuint> & indices) :vertices(vertices), textures(textures), indices(indices) {
		this->init();
	}
private:
	void init();
};

class Model {
public:
	Model(const std::string & filename) :path(filename.substr(0, filename.find_last_of('/'))) {
		this->loadModel();
	}
	void draw();
private:
	std::string path;
	std::vector<Mesh> meshes;
	void loadModel();
};