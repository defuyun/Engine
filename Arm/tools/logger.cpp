#include "logger.h"

std::unique_ptr<tools::logger> Logger = std::make_unique<tools::logger>("error_log.txt");

tools::logger::logger(const char * outfile) : out(outfile, std::fstream::out) {}

tools::logger::~logger()
{
	out.close();
}

void tools::logger::log(const std::string & message)
{
	out << message.c_str();
}