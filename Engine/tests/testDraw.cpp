//#include "tests.h"
//
//void drawTriangle() {
//	engine->createWindow("drawTriangle", 400, 400);	
//	GLuint va = engine->createVertexArrayObject();
//
//	shfile triangle_v = { "shaders/triangle_v.glsl", GL_VERTEX_SHADER };
//	shfile triangle_f = { "shaders/triangle_f.glsl", GL_FRAGMENT_SHADER };
//
//	GLuint sid = engine->createProgram("triangle", { triangle_v, triangle_f });
//
//	base ba = engine->createBaseObject(va, "triangle");
//	ba->setMesh(indicesTriangle, sizeof(indicesTriangle) / sizeof(GLfloat));
//	ba->setShaderID(sid);
//	ba->setVerticeStart(0);
//	ba->setVerticeCount(sizeof(indicesTriangle) / (sizeof(GLfloat) * 6));
//	ba->setSeperator({ {0,3}, {3,6}});
//	ba->setLocs({ "pos","col" });
//	ba->setStride(6);
//	ba->bind();
//
//	object obj = engine->createObject<doge::object>(ba);
//	
//	while (engine->isRunning()) {
//		engine->update();
//		engine->draw();
//	}
//}
//
//void drawTriangleElements() {
//	engine->createWindow("drawTriangle", 400, 400);
//	GLuint va = engine->createVertexArrayObject();
//
//	shfile triangle_v = { "shaders/triangle_v.glsl", GL_VERTEX_SHADER };
//	shfile triangle_f = { "shaders/triangle_f.glsl", GL_FRAGMENT_SHADER };
//
//	GLuint sid = engine->createProgram("triangle", { triangle_v, triangle_f });
//
//	base ba = engine->createBaseObject(va, "triangle");
//	ba->setShaderID(sid);
//	ba->setMesh(indicesTriangle, sizeof(indicesTriangle) / sizeof(GLfloat));
//	ba->setElement(indexesTriangle, sizeof(indexesTriangle) / sizeof(GLshort));
//	ba->setElementInterval(0, 6);
//	ba->setSeperator({ {0,3}, {3,6} });
//	ba->setLocs({ "pos","col" });
//	ba->setStride(6);
//	ba->bind();
//	
//	object obj = engine->createObject<doge::object>(ba);
//
//	while (engine->isRunning()) {
//		engine->update();
//		engine->draw();
//	}
//}
//
//void drawSquareTexture() {
//	engine->createWindow("drawSquare", 400, 400);
//	GLuint va = engine->createVertexArrayObject();
//
//	shfile square_v = { "shaders/square_texture_v.glsl", GL_VERTEX_SHADER };
//	shfile square_f = { "shaders/square_texture_f.glsl", GL_FRAGMENT_SHADER };
//	
//	GLuint sid = engine->createProgram("square", {square_v, square_f });
//	engine->addTexture("images/container.jpg", { "testTexture","default",doge::type::TEX2 });
//	engine->addTexture("images/container2.png", { "testTexture","box", doge::type::TEX2 });
//
//	base ba = engine->createBaseObject(va, "square");
//	ba->setShaderID(sid);
//	ba->setMesh(indicesSquareTexture, sizeof(indicesSquareTexture) / sizeof(GLfloat));
//	ba->setVerticeStart(0);
//	ba->setVerticeCount(6);
//	ba->setSeperator({ {0,3}, {3,5} });
//	ba->setLocs({ "pos","tex" });
//	ba->setStride(5);
//	ba->bind();
//	
//	object obj = engine->createObject<doge::object>(ba);
//	object obj2 = engine->createObject<doge::object>(ba);
//
//	obj->addSim({ "testTexture","box",doge::type::TEX2 }, doge::option::OWR);
//	obj2->addSim({ "testTexture","default",doge::type::TEX2 }, doge::option::OWR);
//
//	while (engine->isRunning()) {
//		engine->update();
//		engine->draw();
//	}
//}
//
//void drawTriangleUsingCombinedMesh() {
//	engine->createWindow("drawSquare", 400, 400);
//
//	GLuint tri = engine->createVertexArrayObject();
//	GLuint sq = engine->createVertexArrayObject();
//
//	shfile triangle_v = { "shaders/triangle_v.glsl", GL_VERTEX_SHADER };
//	shfile triangle_f = { "shaders/triangle_f.glsl", GL_FRAGMENT_SHADER };
//
//	shfile square_v = { "shaders/square_texture_v.glsl", GL_VERTEX_SHADER };
//	shfile square_f = { "shaders/square_texture_f.glsl", GL_FRAGMENT_SHADER };
//	
//	GLuint trsid = engine->createProgram("triangle", { triangle_v, triangle_f });
//	GLuint sqsid = engine->createProgram("square", {square_v, square_f });
//	
//	engine->addTexture("iamges/container.jpg", { "testTexture","default",doge::type::TEX2 });
//	engine->addTexture("images/container2.png", { "testTexture","box", doge::type::TEX2 });
//
//	base sqba = engine->createBaseObject(sq, "square");
//	base trba = engine->createBaseObject(tri, "triangle");
//
//	sqba->setShaderID(sqsid);
//	sqba->setMesh(indicesCombined, sizeof(indicesCombined) / sizeof(GLfloat));
//	sqba->setVerticeStart(0);
//	sqba->setVerticeCount(6);
//
//	sqba->setSeperator({ {0,3}, {3,5} });
//	sqba->setLocs({ "pos","tex" });
//	sqba->setStride(5);
//	sqba->bind();
//
//	object sqobj = engine->createObject<doge::object>(sqba);
//	sqobj->addSim({ "testTexture","default",doge::type::TEX2 }, doge::option::OWR);
//
//	trba->shareMesh(sqba);
//	trba->setShaderID(trsid);
//	trba->setVerticeStart(5);
//	trba->setVerticeCount(6);
//	trba->setSeperator({ {0,3}, {3,6}});
//	trba->setLocs({ "pos","col" });
//	trba->setStride(6);
//	trba->bind();
//
//	object trobj = engine->createObject<doge::object>(trba);
//	
//	while (engine->isRunning()) {
//		engine->update();
//		engine->draw();
//	}
//}
//
//void testBuilder() {
//	engine->createWindow("drawTriangle", 400, 400);
//	GLuint va = engine->createVertexArrayObject();
//
//	shfile triangle_v = { "shaders/triangle_v.glsl", GL_VERTEX_SHADER };
//	shfile triangle_f = { "shaders/triangle_f.glsl", GL_FRAGMENT_SHADER };
//
//	GLuint sid = engine->createProgram("triangle", { triangle_v, triangle_f });
//
//	base ba = builder->build("triangle", va, sid, indicesTriangle, sizeof(indicesTriangle) / sizeof(GLfloat), 6, { {0,3},
//	{3,6} }, { "pos","col" });
//
//	object obj = engine->createObject<doge::object>(ba);
//
//	while (engine->isRunning()) {
//		engine->update();
//		engine->draw();
//	}
//}