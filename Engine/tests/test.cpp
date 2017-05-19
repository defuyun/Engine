#include "tests.h"

void drawTriangle() {
	engine->createWindow("drawTriangle", 400, 400);	
	GLuint va = engine->createVertexArrayObject();

	shfile triangle_v = { "tests/triangle_v.glsl", GL_VERTEX_SHADER };
	shfile triangle_f = { "tests/triangle_f.glsl", GL_FRAGMENT_SHADER };

	GLuint sid = engine->createProgram("triangle", { triangle_v, triangle_f });

	base ba = engine->createBaseObject(va, "triangle");
	ba->setMesh(indicesTriangle, sizeof(indicesTriangle) / sizeof(GLfloat));
	ba->setShaderID(sid);
	ba->setVerticeStart(0);
	ba->setVerticeCount(sizeof(indicesTriangle) / (sizeof(GLfloat) * 6));
	ba->setSeperator({ {0,3}, {3,6}});
	ba->setLocs({ "pos","col" });
	ba->setStride(6);
	ba->bind();

	object obj = engine->createObject(ba);
	
	while (engine->isRunning()) {
		engine->draw();
	}
}