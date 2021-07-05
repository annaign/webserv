#ifndef DEFAULTPAGE_HPP
#define DEFAULTPAGE_HPP

#include "webserv.hpp"

class DefaultPage
{
public:
	DefaultPage();
	~DefaultPage();

	static std::string getPage(std::string error, std::string const &text,
							   std::string const &servVersion = WEBSERV_NAME);
};

#endif