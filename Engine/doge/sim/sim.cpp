#include "sim.h"

int doge::shaderIndexManager::getTextureCount() const
{
	return textureCount;
}

int doge::shaderIndexManager::addTextureCount()
{
	++textureCount;
	return textureCount;
}

void doge::shaderIndexManager::addTexture(const std::string & file, const shaderIndexWrapper & si)
{
	GLuint newt;
	glGenTextures(1, &newt);
	set(si.loc_, si.cls_, si.type_, getTextureCount());
	int tw, th;
	unsigned char * image = SOIL_load_image(file.c_str(), &tw, &th, 0, SOIL_LOAD_RGB);

	glActiveTexture(GL_TEXTURE0 + getTextureCount());
	glBindTexture(GL_TEXTURE_2D, newt);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tw, th, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	addTextureCount();

	SOIL_free_image_data(image);
}

