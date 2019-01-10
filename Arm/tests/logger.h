#pragma once

#include <fstream>
#include <memory>

namespace tools
{
	class logger
	{
	private:
		std::fstream out;
	public:
		logger(const char * outfile);
		~logger();
		void log(const std::string & str);
	};
}

extern std::unique_ptr<tools::logger> Logger;