#pragma once

#include <GL/glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <vector>
#include <string>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"

struct Texture {
	GLuint id;
	std::string type;
};

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<GLuint> indices;
	Mesh(const std::vector<Vertex> & vertices, const std::vector<GLuint> & indices, const std::vector<Texture> & textures) :vertices(vertices), textures(textures), indices(indices) {
		this->init();
	}

	void draw(const Shader & shader) const;
private:
	GLuint vao, vbo, ebo;
	void init();
};

class Model {
public:
	Model(const std::string & filename) :path(filename.substr(0, filename.find_last_of('/'))), filename(filename) {
		this->loadModel();
	}

	void draw(const Shader & shader) const;
private:
	std::string path;
	std::string filename;
	std::unordered_map<std::string, GLuint> loadedTexture;
	std::vector<Mesh> meshes;

	void loadModel();

	void processNode(aiNode * node, const aiScene * scene);
	Mesh processMesh(aiMesh * mesh, const aiScene * scene);

	std::vector<Texture> loadTexture(aiMaterial * materials, aiTextureType type, const std::string & typeName);
};