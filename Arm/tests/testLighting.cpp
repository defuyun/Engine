//#include "tests.h"
//#include "../user/uaction.h"
//
//void testlighting()
//{
//	using tools::shaderindex;
//	using tools::getshader;
//
//	engine->createwindow("lighting", 700, 700);
//	gluint cube = engine->createvertexarrayobject();
//	gluint lamp = engine->createvertexarrayobject();
//
//	shfile cubevshader = getshader(shaderindex::v_cube);
//	shfile cubefshader = getshader(shaderindex::f_cube);
//
//	shfile lampvshader = getshader(shaderindex::v_lamp);
//	shfile lampfshader = getshader(shaderindex::f_lamp);
//
//	gluint sid = engine->createprogram("cube", { cubevshader, cubefshader });
//	gluint lampid = engine->createprogram("lamp", { lampvshader, lampfshader });
//
//	int stride = 8;
//
//	base baseobject = engine->createbaseobject(cube, "lightingobject");
//	base baselamp = engine->createbaseobject(lamp, "lamp");
//
//	baseobject->setshaderid(sid);
//	baseobject->setmesh(indicescube, sizeof(indicescube) / sizeof(glfloat));
//	baseobject->setverticestart(0);
//	baseobject->setverticecount((sizeof(indicescube) / sizeof(glfloat)) / stride);
//	baseobject->setstride(stride);
//	baseobject->setseperator({
//		{0,3},
//		{3,6}
//	});
//
//	baseobject->setlocs({
//		"position_",
//		"normal_"
//	});
//	baseobject->bind();
//
//	baselamp->setshaderid(lampid);
//	baselamp->sharemesh(baseobject);
//	baselamp->setseperator({
//		{0,3},
//	});
//
//	baselamp->setlocs({
//		"position_",
//	});
//	baselamp->bind();
//
//	object cubeobj = engine->createobject<doge::object>(baseobject);
//	object lampobj = engine->createobject<doge::object>(baselamp);
//
//	auto sim = engine->getsim();
//	auto mec = engine->getmec();
//
//	mec->setsensitivity(0.2);
//	tools::filltutdata(sim);
//
//	action move = engine->createaction<doge::default_move>();
//	action look = engine->createaction<doge::default_lookaround>();
//	action lmove = engine->createaction<doge::lamp_move>();
//
//	auto lightcolor = siw{ "lightcolor_","default", doge::type::vec3 };
//	auto lightpos = siw{ "lightpos_","default",doge::type::vec3 };
//	auto objectcolor = siw{ "objectcolor_","default", doge::type::vec3 };
//	auto camerapos = siw{ "camerapos_", "default", doge::type::vec3 };
//
//	cubeobj->addsim(lightcolor);
//	cubeobj->addsim(objectcolor);
//	cubeobj->addsim(lightpos);
//	cubeobj->addsim(camerapos);
//
//	lampobj->setpos({ 1.0f,1.0f,0.0f })->setfront({ 0.0f,0.0f,-1.0f })->setup({ 0.0f,1.0f,0.0f });
//	lampobj->setscale({ 0.2,0.2,0.2 });
//	sim->set(lightpos, lampobj->getpos());
//	sim->set(camerapos, engine->getcamera()->getpos());
//
//	move->addobj(engine->getcamera());
//	look->addobj(engine->getcamera());
//	lmove->addobj(lampobj);
//
//	while (engine->isrunning())
//	{
//		engine->update();
//		engine->draw();
//	}
//}