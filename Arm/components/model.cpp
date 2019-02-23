#include "model.h"
#include "stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

GLuint loadTextureFromFile(const std::string & path, const std::string & filename);
void calculateTangentSpace(Vertex & v0, Vertex & v1, Vertex & v2);

bool Model::useGamma = false;

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

	for (int i = 0; i < indice.size(); i += 3) {
		Vertex & v0 = vertex[indice[i]];
		Vertex & v1 = vertex[indice[i + 1]];
		Vertex & v2 = vertex[indice[i + 2]];
	
		calculateTangentSpace(v0, v1, v2);
	}

	this->meshes.push_back(Mesh(vertex,indice,texture));
}

void Model::loadModel() {
	Assimp::Importer importer;
	
	const auto * scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << importer.GetErrorString();
		return;
	}

	processNode(scene->mRootNode, scene, scene->mRootNode->mTransformation);
}

void Model::processNode(aiNode * node, const aiScene * scene, aiMatrix4x4 transform) {
	for (auto i = 0u; i < node->mNumMeshes; i++) {
		auto mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene, transform));
	}

	for (auto i = 0u; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, transform * node->mChildren[i]->mTransformation);
	}
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene, aiMatrix4x4 transform) {
	std::vector<Vertex> vertices;
	for (auto i = 0u; i < mesh->mNumVertices; i++) {
		auto vertex = transform * mesh->mVertices[i];
		auto aNormal = (aiMatrix3x3(transform).Inverse()).Transpose() * mesh->mNormals[i];

		glm::vec3 position{ vertex.x, vertex.y, vertex.z };
		glm::vec3 normal{ aNormal.x, aNormal.y, aNormal.z };
		glm::vec2 texCoord;
		if (mesh->mTextureCoords[0]) {
			texCoord.x = (mesh->mTextureCoords)[0][i].x;
			texCoord.y = (mesh->mTextureCoords)[0][i].y;
		}
		vertices.push_back({ position, normal, texCoord });
	}

	std::vector<GLuint> indices;
	for (auto i = 0u; i < mesh->mNumFaces; i++) {
		const auto & face = mesh->mFaces[i];
		for (auto j = 0u; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	for (int i = 0; i < indices.size(); i += 3) {
		Vertex & v0 = vertices[indices[i]];
		Vertex & v1 = vertices[indices[i + 1]];
		Vertex & v2 = vertices[indices[i + 2]];

		calculateTangentSpace(v0, v1, v2);
	}

	std::vector<Texture> textures;
	auto materials = scene->mMaterials[mesh->mMaterialIndex];
	
	auto diffuseTextures = loadTexture(materials, aiTextureType_DIFFUSE, "diffuse");
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
	auto specularTextures = loadTexture(materials, aiTextureType_SPECULAR, "specular");
	textures.insert(textures.end(),specularTextures.begin(),specularTextures.end());
	auto normalMapTextures = loadTexture(materials, postfix == "obj" ? aiTextureType_HEIGHT : aiTextureType_NORMALS, "normal");
	textures.insert(textures.end(),normalMapTextures.begin(),normalMapTextures.end());
	
	return Mesh{ vertices, indices, textures };
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

void Model::drawInstance(const Shader & shader, int count) const {
	for (const auto & mesh : meshes) {
		mesh.drawInstance(shader, count);
	}
}

void Model::instanceTranslation(GLuint ibo) const {
	for (const auto & mesh : meshes) {
		mesh.instanceTranslation(ibo);
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

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));
	glEnableVertexAttribArray(4);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Mesh::instanceTranslation(GLuint ibo) const {
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, ibo);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) sizeof(glm::vec4));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) (2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) (3 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glBindVertexArray(0);
}

void Mesh::prepareDraw(const Shader & shader) const {
	shader.use();

	int diffuseCount = 0;
	int specularCount = 0;
	int normalCount = 0;

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
		else if (tex.type == "normal") {
			number = std::to_string(normalCount++);
		}

		GLuint loc = glGetUniformLocation(shader.ID, ("material." + tex.type + number).c_str());
		glUniform1i(loc, (GLint)i);

		glBindTexture(GL_TEXTURE_2D, tex.id);
	}
}
void Mesh::draw(const Shader & shader) const {
	this->prepareDraw(shader);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(0);
}

void Mesh::drawInstance(const Shader & shader, int count) const {
	this->prepareDraw(shader);
	glBindVertexArray(vao);
	glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0, count);
	glBindVertexArray(0);
	glActiveTexture(0);
}

std::ofstream & toString(std::ofstream & file, const glm::vec3 & vec) {
	file << vec.x << " " << vec.y << " " << vec.z;
	return file;
}

std::ofstream & toString(std::ofstream & file, const glm::vec2 & vec) {
	file << vec.x << " " << vec.y;
	return file;
}

void Model::out(const std::string & filename) const{
	std::ofstream file;
	file.open(filename);

	std::vector<Vertex> totalVertex;
	std::vector<GLuint> totalIndices;

	for (auto & mesh : meshes) {
		totalVertex.insert(totalVertex.end(), mesh.vertices.begin(), mesh.vertices.end());
		totalIndices.insert(totalIndices.end(), mesh.indices.begin(), mesh.indices.end());
	}

	file << "vertices\n";
	for (auto & vertex : totalVertex) {
		toString(file, vertex.pos);
		file << " ";
		toString(file, vertex.normal);
		file << " ";
		toString(file, vertex.texCoord);
		file << "\n";
	}

	file << "indices\n";
	for (auto indice : totalIndices) {
		file << indice << '\n';
	}

	file.close();
}

GLuint loadTextureFromFile(const std::string & path, const std::string & filename) {
	GLuint textureId;
	glGenTextures(1, &textureId);


	int width, height, nrChannel;
	auto data = stbi_load((path + '/' + filename).c_str(), &width, &height, &nrChannel,0);

	if (!data) {
		std::cout << "Failed to load texture " + path + '/' + filename << '\n';
		return -1;
	}
	
	GLenum format = -1;
	switch (nrChannel) {
	case 1:
		format = GL_RED;
		break;
	case 3:
		format = Model::useGamma ? GL_SRGB : GL_RGB;
		break;
	case 4:
		format = Model::useGamma ? GL_SRGB_ALPHA : GL_RGBA;
		break;
	default:
		format = GL_RGB;
	}

	GLint param = (format == GL_RGBA || format == GL_SRGB_ALPHA)  ? GL_CLAMP_TO_EDGE : GL_REPEAT;

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	return textureId;
}
#define USE_TEX_FOR_TANGENT
void calculateTangentSpace(Vertex & v0, Vertex & v1, Vertex & v2) {

#ifdef  USE_TEX_FOR_TANGENT
	float deltaU_0 = (v2.texCoord - v0.texCoord)[0];
	float deltaV_0 = (v2.texCoord - v0.texCoord)[1];

	float deltaU_1 = (v1.texCoord - v0.texCoord)[0];
	float deltaV_1 = (v1.texCoord - v0.texCoord)[1];

	glm::vec3 e0 = v2.pos - v0.pos;
	glm::vec3 e1 = v1.pos - v0.pos;

	glm::mat2 iuv = glm::inverse(glm::transpose(glm::mat2(glm::vec2(deltaU_0, deltaV_0), glm::vec2(deltaU_1, deltaV_1))));
	glm::mat3x2 E = glm::transpose(glm::mat2x3(e0, e1));

	glm::mat2x3 tb = glm::transpose(iuv * E);

	glm::vec3 tangent = glm::normalize(tb[0]);
	glm::vec3 bitangent = glm::normalize(tb[1]);
	
	v0.tangent = v1.tangent = v2.tangent = tangent;
	v0.bitangent = v1.bitangent = v2.bitangent = bitangent;
#else
	glm::vec3 tangent = glm::normalize(v2.pos - v0.pos);
	glm::vec3 normal = glm::normalize(v0.normal);

	tangent = glm::normalize(tangent - glm::dot(tangent, normal) * normal);

	glm::vec3 bitangent = glm::normalize(glm::cross(tangent, normal));

	v0.tangent = v1.tangent = v2.tangent = tangent;
	v0.bitangent = v1.bitangent = v2.bitangent = bitangent;
#endif
}
