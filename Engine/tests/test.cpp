#include "tests.h"

void drawTriangle() {
	engine->createWindow("drawTriangle", 400, 400);	
	GLuint va = engine->createVertexArrayObject();
	base ba = engine->createBaseObject(va, "triangle");
	ba->setMesh(indicesTriangle, sizeof(indicesTriangle) / sizeof(GLfloat));
	
	object obj = engine->createObject(ba);
	

}