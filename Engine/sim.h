#pragma once
#include "header.h"

/*
	shaderIndexManager is a linker between code and shader, all of the variables 
	in the shader are stored in the shaderIndexManager
*/

namespace doge {
	enum type : unsigned int;

	struct shaderIndexWrapper {
		std::string loc, cls;
		type t;
		shaderIndexWrapper(const std::string & l, const std::string & c, type tp) :
			loc(l), cls(c), t(tp) {
			// Nothing here
		}
	};

	template<typename T> struct shaderDataWrapper{
		std::string loc, cls;
		T data;
		shaderDataWrapper(const std::string & l, const std::string & c, const T & d) :
			loc(l), cls(c), data(d) {
			// Nothing here
		}
	};
	
	#include "enum.temp"
	class shaderIndexManager {
	private:
		#include "simtype.temp"
	public:
		template<typename T> const std::unordered_map<std::string, std::unordered_map<std::string, T>> & getStorage() const {
			std::runtime_error("[SIM] using the implicit template for getStorage, shouldn't happen\n");
			return std::unordered_map<std::string, std::unordered_map<std::string, T>>();
			// this function should never be used
		}

		#include "simauto.temp"

		template<typename T> void setInput(const std::string & loc, const std::string & cls, const T & vec) {
			this->getStorage<T>()[loc][cls] = vec;
		}

		template<typename T> T get(const shaderIndexWrapper & siw) const{
			const auto & map = this->getStorage<T>();
			auto it = map.find(siw.loc);
			if (it == map.end()) {
				std::runtime_error("[SIM] trying to get an unkown location in get: " + siw.loc + "\n");
			}

			auto it2 = it->second.find(siw.cls);
			if (it2 == it->second.end()) {
				std::runtime_error("[SIM] trying to get an unkown clas under loc: " + siw.loc + " which is: " + siw.cls + "\n");
			}

			return it2->second;
		}

		template<typename T>
		void add(const shaderDataWrapper<T> & sd) {
			this->setInput(sd.loc, sd.cls, sd.data);
		}

		template<typename T, typename... Args> 
		void add(const shaderDataWrapper<T> & sd, Args... args) {
			this->add(sd);
			this->add(args...);
		}

		template<typename T>
		void add(const std::vector<shaderDataWrapper<T>> & sd) {
			for (auto & sds : sd) {
				this->setInput(sds.loc, sds.cls, sds.data);
			}
		}

		template<typename T, typename... Args> 
		void add(const std::vector<shaderDataWrapper<T>> & sd, Args... args) {
			this->add(sd);
			this->add(args...);
		}
	};

	typedef shaderIndexWrapper siw;
	template<typename T> using sdw = shaderDataWrapper<T>;
};

namespace std {	
	template<> struct std::hash<doge::shaderIndexWrapper> {
		std::size_t operator()(const doge::shaderIndexWrapper & si) const {
			return std::hash<string>{}(si.loc + "#" + si.cls);
		}
	};
}