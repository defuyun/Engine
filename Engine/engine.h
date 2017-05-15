#ifndef H_ENG_H
#define H_ENG_H

#include "header.h"
#include "shader.h"
#include "display.h"
#include "sim.h"
#include "object.h"

namespace doge {
	class Engine {
	private:
		std::unique_ptr<display> _window;
		std::unique_ptr<shader> _shader;
		std::unique_ptr<shaderIndexManager> _sim;

	public:
		// constructor for the engine, 
		// initialize glfw which is a library for creating the actual window
		// creates the sim
		Engine();
		
		// creates the actual window for display, 
		// attach that window as our display buffer
		// set the inputs, default sticky key and hidden cursor
		// init glew so we can start using opengl
		// sets the default background color to black
		// enables depth
		void createWindow(const std::string &, int, int);		
		
		// if the window is still running, other words we didn't close the window
		bool isRunning();	

		// ==================== shader class functions ================== //
		GLuint createProgram(const std::string & name, const std::vector<shfile> & files);
		GLuint getPid(const std::string &) const;
	
		// ==================== sim class functions ===================== //
		template<typename T> T getSim(const siw & key) const { return _sim->get<T>(siw); }
		template<typename... T> void addSim(T... keyvalPair) { _sim->add(keyvalPair...);  }
	};

	extern std::unique_ptr<Engine> engine;
};

typedef std::unique_ptr<doge::shader> shader;
typedef std::unique_ptr<doge::shaderIndexManager> sim;

using doge::engine;
using doge::siw;
using doge::sdw;

#endif