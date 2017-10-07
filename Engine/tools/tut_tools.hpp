#pragma once

#include <vector>
#include "../doge/shader.h"
#include "../doge/sim/sim.h"

namespace tools
{
	enum shaderIndex
	{
		V_CUBE = 0,
		F_CUBE = 1,
		V_LAMP = 2,
		F_LAMP = 3
	};
	
	extern const std::vector<doge::shfile> shaderPaths;

	inline doge::shfile getShader(shaderIndex index)
	{
		return shaderPaths[index];
	}

	void fillTutData(doge::shaderIndexManager * sim);
}