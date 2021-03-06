#pragma once
#include "../header.h"
#include "sim/sim.h"

namespace doge {
	class object;
	class camera;

	class baseObject {
	private:
		GLuint va, vb, ve, pid;
		bool hasElement;
		int stride;
		int elementStart, elementEnd;
		int verticeStart, verticeCount;
		std::vector<GLfloat> mesh;
		std::vector<GLshort> elements;
		std::vector<std::pair<int,int>> seperator;
		std::vector<std::string> locs;
		
		int totalMeshSize;
		int totalElementSize;

		std::unordered_map<std::string,siw> defaultSims;
		void bind(const std::vector<std::pair<int,int>> & seps, const std::vector<std::string> & locs) const;
	public:
		baseObject(GLuint a);
		GLuint getVertexArrayObject() const;
		void bind() const;
		void setMesh(const GLfloat * mesh, int size);
		void setVerticeStart(int start);
		void setVerticeCount(int count);
		void setElement(const GLshort * const elem, int size);
		void setElementInterval(int start, int end);
		void shareMesh(const std::shared_ptr<baseObject> & other);
		void setShaderID(GLuint id);
		void setSeperator(const std::vector<std::pair<int,int>> & seps);
		void setLocs(const std::vector<std::string> & locs);
		void setDefaultSims(const std::vector<siw> & sims);
		void setStride(int stride);
		void addSim(const siw & si);
		void addSim(const std::vector<siw> & sims);
		void removeSim(const std::string & loc_);
		void removeSim(const siw & loc_);
		friend class object;
	};

	class object {
	private:
		std::unique_ptr<shaderIndexManager> & _sim;
		std::shared_ptr<baseObject> _bo;
		std::unordered_map<std::string, siw> overwriteSims;
		std::unordered_map<std::string, siw> excludeSims;
	
		bool _draw, _alive;
		glm::vec3 front, pos, up, scale, rotate;
		GLfloat angle;	
		// I added a middle layer between sim use call in case I need to do checking on the siw in the future
		void useSim(const siw &) const;
	public:
		enum opt {EXC, OWR};

		object(std::unique_ptr<shaderIndexManager> & sim, const std::shared_ptr<baseObject> & bo) :
			_sim(sim), _bo(bo), front(glm::vec3(0.0f,0.0f,0.0f)), pos(glm::vec3(0.0f,0.0f,0.0f)), 
			up(glm::vec3(0.0f,1.0f,0.0f)), scale(glm::vec3(1.0f,1.0f,1.0f)), rotate(glm::vec3(1.0f,0.0f,0.0f)),
			angle(0.0f){
			// Nothing here
			this->setAlive(true);
			this->setDraw(true);
		}

		object * setFront(const glm::vec3 &);
		object * setPos(const glm::vec3 &);
		object * setUp(const glm::vec3 &);
		object * setRotate(const glm::vec3 &);
		object * setScale(const glm::vec3 &);
		object * setAngle(GLfloat angle);
		object * setDraw(bool);
		object * setAlive(bool);
	
		glm::vec3 getFront() const;
		glm::vec3 getPos() const;
		glm::vec3 getUp() const;
		glm::vec3 getRotate() const;
		glm::vec3 getScale() const;
		GLfloat getAngle() const;
		bool getDraw() const;
		bool getAlive() const;
		glm::mat4 getModel() const;

		void addSim(const siw & si, opt = OWR);
		void addSim(const std::vector<siw> & sims, opt);
		void removeSim(const std::string & loc_, opt);
		void removeSim(const siw & loc_, opt);
	
		virtual void draw(const std::shared_ptr<camera> & cam) const;
		template<typename T> void setSim(const sdw<T> & sd) {
			this->_sim->add(sd);
		}
		
		bool sanityCheck(std::stringstream & log) const;
	};

	class camera : public object {
	private:
		GLfloat fov, aspect, near, far;
	public:
		camera(std::unique_ptr<shaderIndexManager> & sim, const std::shared_ptr<baseObject> & bo) : object(sim, bo), 
		fov(45.0f), aspect(1.0f), near(0.1f), far(100.0f){
			this->setAlive(false);
			this->setDraw(false);
		}

		camera * setFov(GLfloat fov);
		camera * setAspect(GLfloat aspect);
		camera * setNear(GLfloat near);
		camera * setFar(GLfloat far);

		glm::mat4 getProject() const;
		glm::mat4 getView() const;
	};

	typedef std::pair<int, int> interval;
	typedef std::pair<int, int> ipair;
	typedef object::opt option;
};