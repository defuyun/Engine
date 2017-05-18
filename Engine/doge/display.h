#pragma once
#include "../header.h"

namespace doge {
	class display {
	public:
		GLFWwindow * window;
		int width, height;
		std::string name;
	
		display(const std::string & name, int w, int h) {
			window = glfwCreateWindow(w, h, name.c_str(), NULL, NULL);
			if (!window) {
				std::runtime_error("failed to create window\n");
			}
		}

		~display() {
			glfwDestroyWindow(window);
		}
	};
};
