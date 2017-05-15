#pragma once
#include "header.h"
#include "sim.h"

namespace doge {
	class object;

	class baseObject {
	private:
		GLuint va, vb, ve, pid;
		bool hasElement;
		int stride;
		int elementStart, elementEnd;
		int verticeStart, verticeEnd;
		std::vector<GLfloat> mesh;
		std::vector<GLshort> elements;
		std::vector<std::pair<int,int>> seperator;
		std::vector<std::string> locs;
	
		std::unordered_map<std::string,siw> defaultSims;
		void bind(std::vector<std::pair<int,int>> & seps, std::vector<std::string> & locs);
	public:
		baseObject(GLuint a);
		void setMesh(const GLfloat * mesh, int size);
		void setInterval(int start, int end);
		void setElement(const GLshort * const elem, int size);
		void shareMesh(const std::shared_ptr<baseObject> & other);
		void setShaderID(GLuint id);
		void setSeperator(const std::vector<std::pair<int,int>> & seps);
		void setLocs(const std::vector<std::string> & locs);
		void setDefaultSims(const std::vector<siw> & sims);
		void addSim(const siw & si);
		void removeSim(const std::string & loc);
		void removeSim(const siw & loc);
		friend class object;
	};

	class object {
	private:
		std::unique_ptr<shaderIndexManager> & _sim;
		std::shared_ptr<baseObject> _bo;
		std::unordered_set<siw> overwriteSims;
		std::vector<int> overwriteSeps;
		std::vector<std::string> overwriteLocs;
	public:
		object(std::unique_ptr<shaderIndexManager> & sim, const std::shared_ptr<baseObject> & bo) :
			_sim(sim), _bo(bo) {
			// Nothing here			
		}
	};
};