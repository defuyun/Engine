#include "mesh.h"

GLfloat indicesTriangle[] = {
		-1.0f, 1.0f, 0.0f,		1.0f,0.0f,0.0f,
		-1.0f,-1.0f, 0.0f,		0.0f,1.0f,0.0f,
		0.0f,-1.0f, 0.0f,		0.0f,0.0f,1.0f,

		1.0f, 1.0f, 0.0f,		1.0f,0.0f,0.0f,
		1.0f, -1.0f, 0.0f,		0.0f,1.0f,0.0f,
		0.0f,-1.0f, 0.0f,		0.0f,0.0f,1.0f
};

GLshort indexesTriangle[] = { 0,1,2,3,4,2 };