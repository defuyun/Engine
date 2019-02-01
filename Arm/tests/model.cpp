#include "model.h"
#include "logger.h"
#include "stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GLuint loadTextureFromFile(const std::string & path, const std::string & filename);

void Model::load() {
	if (postfix == "vertices") {
		loadFile();
	}
	else {
		loadModel();
	}
}

void Model::loadFile() {
	std::ifstream file(filename);
	std::string line;
	enum Section {
		vertices,
		indices,
		textures
	};

	Section current;
	std::vector<Vertex> vertex;
	std::vector<GLuint> indice;
	std::vector<Texture> texture;

	while (std::getline(file, line)) {
		if (line == "vertices")
			current = Section::vertices;
		else if (line == "indices")
			current = Section::indices;
		else if (line == "textures")
			current = Section::textures;
		else {
			std::stringstream streamer(line);
			if (current == Section::textures) {
				std::string filename, type;
				streamer >> filename >> type;
				texture.push_back({loadTextureFromFile(filename), type});
			}
			else if (current == Section::indices) {
				GLuint i;
				streamer >> i;
				indice.push_back(i);
			}
			else if (current == Section::vertices) {
				Vertex v;
				streamer >> v.pos.x >> v.pos.y >> v.pos.z >> v.normal.x >> v.normal.y >> v.normal.z >> v.texCoord.x >> v.texCoord.y;
				vertex.push_back(v);
			}
		}
	}

	this->meshes.push_back(Mesh(vertex,indice,texture));
}

void Model::loadModel() {
	Assimp::Importer importer;
	
	const auto * scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Logger->log(importer.GetErrorString());
		return;
	}

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode * node, const aiScene * scene) {
	for (auto i = 0u; i < node->mNumMeshes; i++) {
		auto mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (auto i = 0u; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene) {
	std::vector<Vertex> vertex;
	for (auto i = 0u; i < mesh->mNumVertices; i++) {
		glm::vec3 position{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		glm::vec3 normal{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		glm::vec2 texCoord;
		if (mesh->mTextureCoords[0]) {
			texCoord.x = (mesh->mTextureCoords)[0][i].x;
			texCoord.y = (mesh->mTextureCoords)[0][i].y;
		}
		vertex.push_back({ position, normal, texCoord });
	}

	std::vector<GLuint> indices;
	for (auto i = 0u; i < mesh->mNumFaces; i++) {
		const auto & face = mesh->mFaces[i];
		for (auto j = 0u; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	std::vector<Texture> textures;
	auto materials = scene->mMaterials[mesh->mMaterialIndex];
	
	auto diffuseTextures = loadTexture(materials, aiTextureType_DIFFUSE, "diffuse");
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
	auto specularTextures = loadTexture(materials, aiTextureType_SPECULAR, "specular");
	textures.insert(textures.end(),specularTextures.begin(),specularTextures.end());
	
	return Mesh{ vertex, indices, textures };
}

GLuint Model::loadTextureFromFile(const std::string & filename) {
	if (this->loadedTexture.count(filename.c_str())) {
		return loadedTexture[filename];
	}
	else {
		return loadedTexture[filename] = ::loadTextureFromFile(path, filename);
	}
}

std::vector<Texture> Model::loadTexture(aiMaterial * materials, aiTextureType type, const std::string & typeName) {
	std::vector<Texture> textures;
	for (auto i = 0u; i < materials->GetTextureCount(type); i++) {
		aiString filename;
		materials->GetTexture(type, i, &filename);
		
		GLuint textureId = this->loadTextureFromFile(filename.C_Str());
		textures.push_back({ textureId, typeName });
	}

	return textures;
}

void Model::draw(const Shader & shader) const {
	for (const auto & mesh : meshes) {
		mesh.draw(shader);
	}
}

void Mesh::init() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Mesh::draw(const Shader & shader) const {
	
	shader.use();

	int diffuseCount = 0;
	int specularCount = 0;

	for (size_t i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + (GLenum)i);
		const auto & tex = textures[i];
		std::string number;
		if (tex.type == "diffuse") {
			number = std::to_string(diffuseCount++);
		}
		else if (tex.type == "specular") {
			number = std::to_string(specularCount++);
		}

		GLuint loc = glGetUniformLocation(shader.ID, ("material." + tex.type + number).c_str());
		glUniform1i(loc, (GLint)i);

		glBindTexture(GL_TEXTURE_2D, tex.id);
	}

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(0);
}

GLuint loadTextureFromFile(const std::string & path, const std::string & filename) {
	GLuint textureId;
	glGenTextures(1, &textureId);


	int width, height, nrChannel;
	auto data = stbi_load((path + '/' + filename).c_str(), &width, &height, &nrChannel,0);

	if (!data) {
		Logger->log("Failed to load texture" + path + '/' + filename);
		return -1;
	}

	GLenum format = nrChannel == 1 ? GL_RED : nrChannel == 3 ? GL_RGB : GL_RGBA;
	
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	return textureId;
}