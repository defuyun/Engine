#pragma once
#include "../header.h"
#include "object.h"

namespace doge {	
	class mainEngineControl {
	private:
		std::vector<bool> _keyPressed;
		std::vector<bool> _mouseClicked;
		double _cursorX, _cursorY, _preCursorX, _preCursorY;
		double _scrollOffsetX, _scrollOffsetY;
		bool _firstRunFlag;
	public:
		mainEngineControl(int keySize = 1024, int mouseSize = 8);
		void setKeyPressed(int key, int action);
		void setMouseClicked(int button, int action);
		void setCursorPos(double cursorX, double cursorY);
		void setScroll(double scrollY, double scrollX=0);

		bool isKeyPressed(int key) const;
		bool isMouseClicked(int button) const;

		std::pair<double, double> getCursorPos() const;
		std::pair<double, double> getPrevCursor() const;
		std::pair<double, double> getScroll() const;

		double getPitch(double sensitivity) const;
		double getYaw(double sensitivity) const;
	};

	class action {
	private:
		std::unordered_set<std::shared_ptr<doge::object>> boundObj;
	public:
		virtual void execute(std::unique_ptr<mainEngineControl> & mec) const = 0;
		virtual void check(std::unique_ptr<mainEngineControl> & mec) const = 0;
		virtual void addObj(const std::shared_ptr<doge::object> & obj);
	};
};

typedef std::unique_ptr<doge::mainEngineControl> mec;
