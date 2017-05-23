#include "tests.h"

void moveAround() {
	engine->createWindow("move", 500, 500);
	GLuint cube = engine->createVertexArrayObject();

	shfile cubev = { "cube_v.glsl", GL_VERTEX_SHADER };
	shfile cubef = { "cube_f.glsl", GL_FRAGMENT_SHADER };
	
	GLuint sid = engine->createProgram("cube", { cubev, cubef });
	engine->addTexture("images/container.jpg", {"testTexture", "default", doge::type::TEX2});

	base ba = builder->build("triangle", cube, sid, indicesCube, sizeof(indicesCube) / sizeof(GLfloat), 8,
	{ {0,3}, {7,8} }, { "pos", "tex" });
}