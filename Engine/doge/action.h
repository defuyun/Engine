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

	class move : public action {
	private:
		float _moveSpeed;
		int _front, _back, _left, _right;
	public:
		virtual void execute(std::unique_ptr<mainEngineControl> & mec, std::shared_ptr<doge::object> & obj);
		virtual bool check(std::unique_ptr<mainEngineControl> & mec, const std::shared_ptr<doge::object> & obj) const;
		void setMoveSpeed(float moveSpeed);
		void setKeyBinding(int front,int back,int left,int right);
	};

	class lookAround : public action {	
		virtual void execute(std::unique_ptr<mainEngineControl> & mec, std::shared_ptr<doge::object> & obj);
		virtual bool check(std::unique_ptr<mainEngineControl> & mec, const std::shared_ptr<doge::object> & obj) const;
	};
};
