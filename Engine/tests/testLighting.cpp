#include "tests.h"

void testLighting()
{
	using tools::shaderIndex;
	using tools::getShader;

	engine->createWindow("lighting", 700, 700);
	GLuint cube = engine->createVertexArrayObject();
	GLuint lamp = engine->createVertexArrayObject();

	shfile cubeVShader = getShader(shaderIndex::V_CUBE);
	shfile cubeFShader = getShader(shaderIndex::F_CUBE);

	shfile lampVShader = getShader(shaderIndex::V_LAMP);
	shfile lampFShader = getShader(shaderIndex::F_LAMP);

	GLuint sid = engine->createProgram("cube", {cubeVShader, cubeFShader});
	GLuint lampID = engine->createProgram("lamp", {lampVShader, lampFShader});

	int stride = 8;
	
	base baseObject = engine->createBaseObject(cube, "lightingObject");
	base baseLamp = engine->createBaseObject(lamp, "lamp");

	baseObject->setShaderID(sid);
	baseObject->setMesh(indicesCube, sizeof(indicesCube) / sizeof(GLfloat));
	baseObject->setVerticeStart(0);
	baseObject->setVerticeCount((sizeof(indicesCube) / sizeof(GLfloat)) / stride);
	baseObject->setStride(stride);
	baseObject->setSeperator({
		{0,3},
		{3,6}
	});

	baseObject->setLocs({
		"position_",
		"normal_"
	});
	baseObject->bind();

	baseLamp->setShaderID(lampID);
	baseLamp->shareMesh(baseObject);
	baseLamp->setSeperator({
		{0,3},
	});

	baseLamp->setLocs({
		"position_",
	});
	baseLamp->bind();

	object cubeObj = engine->createObject<doge::object>(baseObject);
	object lampObj = engine->createObject<doge::object>(baseLamp);

	auto sim = engine->getSim();
	auto mec = engine->getMec();
	
	mec->setSensitivity(0.2);
	tools::fillTutData(sim);

	action move = engine->createAction<doge::default_move>();
	action look = engine->createAction<doge::default_lookAround>();

	auto lightColor = siw{ "lightColor_","default", doge::type::VEC3 };
	auto lightPos = siw{ "lightPos_","default",doge::type::VEC3 };
	auto objectColor = siw{ "objectColor_","default", doge::type::VEC3 };

	cubeObj->addSim(lightColor);
	cubeObj->addSim(objectColor);
	cubeObj->addSim(lightPos);

	lampObj->setPos({1.0,1.0,0.0});
	lampObj->setScale({ 0.2,0.2,0.2 });
	sim->set(lightPos, lampObj->getPos());

	move->addObj(engine->getCamera());
	look->addObj(engine->getCamera());

	while (engine->isRunning())
	{
		engine->update();
		engine->draw();
	}
}