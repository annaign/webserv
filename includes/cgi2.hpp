#ifndef CGI2_HPP
#define CGI2_HPP

#include "webserv.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "base64.hpp"

class Client;

class CGI2
{
private:
	Client client_;
	ConfigLocation location_;
	std::string *ptrFile;
	std::string *ptrCgiFile;
	char *args_[4];
	char **env_;

	std::map<std::string, std::string> envMap_;
	std::string envir_;

	std::string body_;

public:
	CGI2(Client const &client, ConfigLocation location, std::string const &path);
	~CGI2();

	std::string const &getHeaders() const;

	void createMetaVariables();
	void createHttpMetaVariables();
	void printMetaVariables();
	void createCgiEnv();
	std::string run(std::string &data);
	bool readFile(std::string const &file, std::string &body);
};

#endif