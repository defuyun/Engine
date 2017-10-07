#pragma once
#include "../../header.h"

/*
	shaderIndexManager is a linker between code and shader so it's like a ORM for code variable
	to uniform in shader, all of the variables in the shader are stored in the shaderIndexManager
*/

namespace doge {
	enum type : unsigned int;

	struct shaderDataWrapperT
	{
		virtual void * get() = 0;
	};

	struct shaderDataWrapperBase : public shaderDataWrapperT
	{
		std::string loc_, cls_;
		type type_;

		shaderDataWrapperBase(std::string loc, std::string cls, type id) :
			loc_(loc),
			cls_(cls),
			type_(id) {}

		shaderDataWrapperBase(const shaderDataWrapperBase & copy) :
			loc_(copy.loc_),
			cls_(copy.cls_),
			type_(copy.type_) {}
		
		bool operator==(const shaderDataWrapperBase & rhs) const
		{
			return loc_ == rhs.loc_ && cls_ == rhs.cls_;
		}
		
		virtual void * get() override
		{
			return nullptr;
		}
	};

	template<typename T>
	struct shaderDataWrapper : public shaderDataWrapperBase
	{
		T data;
		shaderDataWrapper(const std::string & l, const std::string & c, type t, const T & d) :
			shaderDataWrapperBase(l, c, t), data(d) {
			// Nothing here
		}

		virtual void * get() override
		{
			return static_cast<void *>(this);
		}
	};
	
	namespace converter
	{
		template<typename T>
		shaderDataWrapper<T> * get(void * data)
		{
			return static_cast<shaderDataWrapper<T> *>(data);
		}
	}
}

namespace std
{
	template<>
	struct hash<doge::shaderDataWrapperBase>
	{
		std::size_t operator()(const doge::shaderDataWrapperBase & key) const
		{
			return hash<string>()(key.loc_ + "#" + key.cls_);
		}
	};
}

namespace doge {

	typedef shaderDataWrapperBase shaderIndexWrapper;

	#include "enum.temp"
	class shaderIndexManager {
	private:
		int textureCount = 0;
		#include "simtype.temp"
	public:
		int getTextureCount() const;
		int addTextureCount();
		void addTexture(const std::string & file, const shaderIndexWrapper & si);

		template<typename T> std::unordered_map<shaderIndexWrapper, std::shared_ptr<T>> & getStorage() {
			Logger->log("[SIM] should not call implicit template getStorage\n");
			return *(new std::unordered_map<std::string, std::unordered_map<std::string, T>>());
		}

		#include "simauto.temp"

		template<typename T> std::weak_ptr<T> get(const shaderIndexWrapper & si) {
			const auto & map = this->getStorage<T>();
			auto it = map.find(si);
			if (it == map.end()) {
				Logger->log("[SIM] trying to get an unknown location in get: " + si.loc_ + "\n");
			}
			return it->second;
		}

		template<typename T>
		void set(const shaderIndexWrapper & si, const T & data)
		{
			this->setInput(si.loc_, si.cls_, si.type_, data);
		}

		template<typename T>
		void set(const std::string & loc_, const std::string & cls_, type t, const T & data)
		{
			this->setInput(loc_, cls_, t, data);
		}

		template<typename T, typename... Args>
		void set(const std::string & loc_, const std::string & cls_, type t, const T & data, Args... args) {
			this->set(loc_,cls_,t,data);
			this->set(args...);
		}

	private:
		template<typename T>
		void setInput(const std::string & loc_, const std::string & cls_, type t, const T & vec) {
			this->getStorage<T>()[shaderDataWrapperBase(loc_,cls_,t)] = std::make_shared<T>(vec);
		}
	};

	typedef shaderIndexWrapper siw;
	template<typename T> using sdw = shaderDataWrapper<T>;
};

