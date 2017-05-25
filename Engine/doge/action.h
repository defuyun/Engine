#pragma once
#include "../header.h"
#include "control.h"

namespace doge {
	class action {
	private:
		std::unordered_set<std::shared_ptr<doge::object>> boundObjs;
	public:
		virtual void execute(std::unique_ptr<mainEngineControl> & mec, std::shared_ptr<doge::object> & obj) = 0;
		virtual bool check(std::unique_ptr<mainEngineControl> & mec, const std::shared_ptr<doge::object> & obj) const = 0;
		virtual void addObj(const std::shared_ptr<doge::object> & obj);
		std::unordered_set<std::shared_ptr<doge::object>> & getBoundObjs();
	};

	class default_move : public action {
	private:
		float _moveSpeed = 0.1f;
		int _front = GLFW_KEY_W, _back = GLFW_KEY_S, _left = GLFW_KEY_A, _right = GLFW_KEY_D;
	public:
		virtual void execute(std::unique_ptr<mainEngineControl> & mec, std::shared_ptr<doge::object> & obj);
		virtual bool check(std::unique_ptr<mainEngineControl> & mec, const std::shared_ptr<doge::object> & obj) const;
		void setMoveSpeed(float moveSpeed);
		void setKeyBinding(int front,int back,int left,int right);
	};

	class default_lookAround : public action {	
	public:
		virtual void execute(std::unique_ptr<mainEngineControl> & mec, std::shared_ptr<doge::object> & obj);
		virtual bool check(std::unique_ptr<mainEngineControl> & mec, const std::shared_ptr<doge::object> & obj) const;
	};

};

typedef std::shared_ptr<doge::default_lookAround> look;
typedef std::shared_ptr<doge::default_move> move;
