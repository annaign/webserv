#ifndef AUTOINDEX_HPP
#define AUTOINDEX_HPP

#include "webserv.hpp"

class Autoindex
{
public:
	Autoindex();
	~Autoindex();

	std::string getPage(std::string const &path, std::string const &sitePath);

private:
	bool addLinks(std::string const &path, std::string sitePath, std::string &page);
};

#endif