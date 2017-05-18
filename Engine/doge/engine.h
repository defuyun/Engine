#ifndef H_ENG_H
#define H_ENG_H

#include "../header.h"
#include "shader.h"
#include "display.h"
#include "sim/sim.h"
#include "object.h"

namespace doge {
	class Engine {
	private:
		const unsigned int _camva = UINT32_MAX;

		std::unique_ptr<display> _window;
		std::unique_ptr<shader> _shader;
		std::unique_ptr<shaderIndexManager> _sim;
		std::unordered_set<std::shared_ptr<object>> _objs;
		std::unordered_map<std::string, std::shared_ptr<baseObject>> _bobjs;
		std::unordered_set<GLuint> _vao;
		
		std::shared_ptr<camera> _cam;
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
		
		void addTexture(const std::string & file, const siw & loc);
		GLuint createVertexArrayObject();
		std::shared_ptr<baseObject> createBaseObject(GLuint va,const std::string &);
		std::shared_ptr<object> createObject(const std::shared_ptr<baseObject> &);
		
		std::shared_ptr<baseObject> & getBaseObject(const std::string & name);
		std::shared_ptr<camera> & getCamera();

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
typedef std::shared_ptr<doge::baseObject> base;
typedef std::shared_ptr<doge::object> object;
typedef std::shared_ptr<doge::camera> camera;

using doge::engine;
using doge::siw;
using doge::sdw;

#endif