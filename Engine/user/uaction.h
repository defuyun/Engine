#pragma once

#include "../doge/action.h"

namespace doge
{
	class lamp_move : public action
	{
		float _moveSpeed = 0.1f;
		int _front = GLFW_KEY_UP, _back = GLFW_KEY_DOWN, _left = GLFW_KEY_LEFT, _right = GLFW_KEY_RIGHT , _up = GLFW_KEY_Q, _down = GLFW_KEY_E;
		virtual void execute(std::unique_ptr<mainEngineControl> & mec, std::shared_ptr<doge::object> & obj);
		virtual bool check(std::unique_ptr<mainEngineControl> & mec, const std::shared_ptr<doge::object> & obj) const;
	};
}
