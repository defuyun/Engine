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

GLuint loadTextureFromFile(const std::string & path, const std::string & filename);

struct Texture {
	GLuint id;
	std::string type;
};

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<GLuint> indices;
	explicit Mesh(const std::vector<Vertex> & vertices, const std::vector<GLuint> & indices, const std::vector<Texture> & textures) :vertices(vertices), textures(textures), indices(indices) {
		this->init();
	}

	void draw(const Shader & shader) const;
	void drawInstance(const Shader & shader, int count) const;
	void instanceTranslation(GLuint ibo) const;
private:
	void prepareDraw(const Shader & shader) const;
	GLuint vao, vbo, ebo;
	void init();
};

class Model {
public:
	static bool useGamma;

	Model(const std::string & filename) :
		path(filename.substr(0, filename.find_last_of('/'))),
		postfix(filename.substr(filename.find_last_of('.') + 1, filename.size())), 
		filename(filename) {
		this->load();
	}

	void instanceTranslation(GLuint ibo) const;
	void draw(const Shader & shader) const;
	void drawInstance(const Shader & shader, int count) const;  private: std::string postfix; std::string path; std::string filename; std::unordered_map<std::string, GLuint> loadedTexture; std::vector<Mesh> meshes;  void load(); void loadModel(); void loadFile();  void processNode(aiNode * node, const aiScene * scene); Mesh processMesh(aiMesh * mesh, const aiScene * scene); 
	std::vector<Texture> loadTexture(aiMaterial * materials, aiTextureType type, const std::string & typeName);
	GLuint loadTextureFromFile(const std::string & filename);
};
