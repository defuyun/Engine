#pragma once
#include "../header.h"
#include "object.h"

namespace doge {
	class mainEngineControl {
	private:
		std::vector<bool> _keyPressed;
		std::vector<bool> _mouseClicked;
		double _cursorX, _cursorY;
		double _scrollOffsetX, _scrollOffsetY;
		double _pitch, _yaw, _pitchMax, _yawMax, _pitchMin, _yawMin;
		double _sensitivity;
		bool _firstRunFlag;
		void setKeyPressed(int key, int action);
		void setMouseClicked(int button, int action);
		void setCursorPos(double cursorX, double cursorY);
		void setScroll(double scrollX, double scrollY);
		void setPitchInterval(double pitchMin, double pitchMax);
		void setYawInterval(double yawMin, double yawMax);
		void setSensitivity(double sensitivity);
	public:
		mainEngineControl(int keySize = 1024, int mouseSize = 8);
		bool isKeyPressed(int key) const;
		bool isMouseClicked(int button) const;
	
		std::pair<double, double> getCursorPos() const;
		std::pair<double, double> getScroll() const;

		double getPitch() const;
		double getYaw() const;
		friend class Engine;
	};
};

typedef std::unique_ptr<doge::mainEngineControl> mec;
