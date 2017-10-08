#ifndef H_ENG_H
#define H_ENG_H

#include "../header.h"
#include "shader.h"
#include "display.h"
#include "sim/sim.h"
#include "object.h"
#include "control.h"
#include "action.h"

namespace doge {
	class Engine {
	private:
		const unsigned int _camva = UINT32_MAX;

		std::unique_ptr<display> _window;
		std::unique_ptr<shader> _shader;
		std::unique_ptr<shaderIndexManager> & _sim;
		std::unique_ptr<mainEngineControl> _mec;
		std::unordered_set<std::shared_ptr<object>> _objs;
		std::unordered_set<std::shared_ptr<action>> _act;
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
		
		// poll for new events, then for each action in the act set execute the action
		void update();
		void draw() const;

		shaderIndexManager * getSim();
		mainEngineControl * getMec();

		std::shared_ptr<camera> & getCamera();
		
		// ========================== object class funcionts ==================== //
		GLuint createVertexArrayObject();
		std::shared_ptr<baseObject> createBaseObject(GLuint va,const std::string &);
		template<typename T> std::shared_ptr<T> createObject(const std::shared_ptr<baseObject> & bobj) {
			std::shared_ptr<T> newobj(new T(this->_sim, bobj));
			this->_objs.insert(newobj);
			return newobj;
		}
		
		std::shared_ptr<baseObject> & getBaseObject(const std::string & name);
		
		// ==================== shader class functions ================== //
		GLuint createProgram(const std::string & name, const std::vector<shfile> & files);
		GLuint getPid(const std::string &) const;
	
		// ==================== sim class functions ===================== //
		template<typename T> T getSim(const siw & key) const { return _sim->get<T>(siw); }
		template<typename... T> void addSim(T... keyvalPair) { _sim->add(keyvalPair...);  }
	
		// =========================== engine control ======================== //
		void setControlPitchInterval(double pitchMin, double pitchMax);
		void setControlYawInterval(double yawMin, double yawMax);
		void setControlSensitivity(double sensitivity);

		// ============================= actions ================================= //
		template<typename T> std::shared_ptr<T> createAction() {	
			std::shared_ptr<T> newAct = std::make_shared<T>();
			this->_act.insert(newAct);
			return newAct;
		}
	
		// ============================ key call backs ========================== //
		static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode);
		static void cursor_callback(GLFWwindow * window, double posx, double posy);
		static void scroll_callback(GLFWwindow * window, double offx, double offy);
	};
	
	extern std::unique_ptr<Engine> engine;
};

typedef std::unique_ptr<doge::shader> shader;
typedef std::unique_ptr<doge::shaderIndexManager> sim;
typedef std::unique_ptr<doge::mainEngineControl> control;
typedef std::shared_ptr<doge::baseObject> base;
typedef std::shared_ptr<doge::object> object;
typedef std::shared_ptr<doge::camera> camera;
typedef std::shared_ptr<doge::action> action;

using doge::engine;
using doge::siw;
using doge::sdw;
using doge::shfile;
using doge::interval;
using doge::type;

#endif