#include "tests.h"

void drawTriangle() {
	engine->createWindow("drawTriangle", 400, 400);	
	GLuint va = engine->createVertexArrayObject();

	shfile triangle_v = { "triangle_v.glsl", GL_VERTEX_SHADER };
	shfile triangle_f = { "triangle_f.glsl", GL_FRAGMENT_SHADER };

	GLuint sid = engine->createProgram("triangle", { triangle_v, triangle_f });

	base ba = engine->createBaseObject(va, "triangle");
	ba->setMesh(indicesTriangle, sizeof(indicesTriangle) / sizeof(GLfloat));
	ba->setShaderID(sid);
	object obj = engine->createObject(ba);
	

}