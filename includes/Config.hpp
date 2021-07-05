#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "webserv.hpp"
#include "ConfigLocation.hpp"
#include "ConfigServer.hpp"

#define BUFFER_SIZE 1024

class ConfigServer;

class Config
{
private:
	std::string errors_;
	std::string config_;

	bool log_error_;
	bool log_access_;
	std::vector<ConfigServer> servers_;
	std::map<std::string, ConfigServer> serversMap_; //"ip:port" -> server

	Config();

protected:
	bool readFile(std::string const &configFile);
	bool parseLocation(ConfigServer &serv, std::vector<std::string> &words, std::vector<std::string>::const_iterator &word);
	bool parseServer(std::vector<std::string> &words, std::vector<std::string>::const_iterator &word);
	void parseConfig();
	void checkConfigErrors();

public:
	Config(std::string const &configFile);
	Config(Config const &src);
	virtual ~Config();

	Config &operator=(Config const &rhs);

	bool setLogError(std::string const &log);
	bool setLogAccess(std::string const &log);
	bool setServers(ConfigServer const &server);

	std::string const &getErrors() const;
	bool getLogError() const;
	bool getLogAccess() const;
	std::vector<ConfigServer> const &getServers() const;
	std::map<std::string, ConfigServer> const &getServersMap() const;

	static bool delimiter(char ch);
	static bool notdelimiter(char ch);
	std::vector<std::string> splitWords(const std::string &str);
};

#endif