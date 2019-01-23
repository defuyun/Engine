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

struct Rotation{
	glm::vec3 axis = glm::vec3(1.0f);
	float angle = 0.0f;
};

struct Model {
	glm::vec3 translate = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	Rotation rotate;
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
	explicit Mesh(const std::vector<Vertex> & vertices, const std::vector<GLuint> & indices, const std::vector<Texture> & textures) :vertices(vertices), textures(textures), indices(indices) {
		this->init();
	}

	void draw(const Shader & shader) const;
private:
	GLuint vao, vbo, ebo;
	void init();
};

class Object {
private:
	std::vector<Mesh> meshes;
public:
	std::string name;
	std::vector<std::shared_ptr<Object>> children;
	Model model;

	Object(const std::string & name = "Anon") : name(name){}
	
	void addChild(const std::shared_ptr<Object> & child) {
		children.push_back(child);
	}

	void addMesh(const Mesh & mesh) {
		meshes.push_back(mesh);
	}

	void draw(const Shader & shader, const glm::mat4 & parentModel = glm::mat4(1.0f)) const;
};

class Scene {
public:
	Scene(const std::string & filename) :
		path(filename.substr(0, filename.find_last_of('/'))),
		postfix(filename.substr(filename.find_last_of('.') + 1, filename.size())), 
		filename(filename) {
		this->load();
	}

	void draw(const Shader & shader) const;
	std::vector<std::shared_ptr<Object>> objects;
private:
	std::string postfix;
	std::string path;
	std::string filename;
	std::unordered_map<std::string, GLuint> loadedTexture;

	void load();
	void loadScene();
	void loadFile();

	void processNode(const std::shared_ptr<Object> & parent, aiNode * node, const aiScene * scene);
	Mesh processMesh(aiMesh * mesh, const aiScene * scene);

	std::vector<Texture> loadTexture(aiMaterial * materials, aiTextureType type, const std::string & typeName);
	GLuint loadTextureFromFile(const std::string & filename);
};