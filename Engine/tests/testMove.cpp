//#include "tests.h"
//
//void moveAround() {
//	engine->createWindow("move", 780, 780);
//	GLuint cube = engine->createVertexArrayObject();
//
//	shfile cubev = { "shaders/cube_v.glsl", GL_VERTEX_SHADER };
//	shfile cubef = { "shaders/cube_f.glsl", GL_FRAGMENT_SHADER };
//	
//	GLuint sid = engine->createProgram("cube", { cubev, cubef });
//	engine->addTexture("images/container.jpg", {"testTexture", "default", doge::type::TEX2});
//
//	base ba = builder->build("triangle", cube, sid, indicesCube, sizeof(indicesCube) / sizeof(GLfloat), 8,
//	{ {0,3}, {6,8} }, { "pos", "tex" });
//
//	object obj = engine->createObject<doge::object>(ba);
//	obj->addSim({"testTexture","default", doge::type::TEX2}, doge::option::OWR);
//
//	action m = engine->createAction<doge::default_move>();
//	action l = engine->createAction<doge::default_lookAround>();
//
//	m->addObj(engine->getCamera());
//	l->addObj(engine->getCamera());
//
//	while (engine->isRunning()) {
//		engine->update();
//		engine->draw();
//	}
//}