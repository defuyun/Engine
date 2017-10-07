#include "tut_tools.hpp"

namespace tools
{
	const std::vector<doge::shfile> shaderPaths = {
		{ "shaders/v_lightingCube.glsl", GL_VERTEX_SHADER },
		{ "shaders/f_lightingCube.glsl", GL_FRAGMENT_SHADER },
		{ "shaders/v_lamp.glsl", GL_VERTEX_SHADER },
		{ "shaders/f_lamp.glsl", GL_FRAGMENT_SHADER },
	};
	
	void fillTutData(doge::shaderIndexManager * sim)
	{
		sim->set(
			"lightColor_", "default", doge::type::VEC3, glm::vec3{1.0f,1.0f,1.0f},
			"objectColor_", "default", doge::type::VEC3, glm::vec3{1.0f,1.0f,1.0f},
			"lightPos_", "default", doge::type::VEC3, glm::vec3{0.0f,0.0f,0.0f}
		);
	}
}