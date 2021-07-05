#ifndef CONFIGSERVER_HPP
#define CONFIGSERVER_HPP

#include "webserv.hpp"


class ConfigLocation;

class ConfigServer
{
private:
	sockaddr_in sockaddr_;
	std::string serverName_;
	std::string serverIp_;
	std::map<std::string, std::string> errorCodes_; // <code, url>
	size_t clientBodyBufferSize_;
	std::string root_;
	std::vector<ConfigLocation> locations_;
	std::map<std::string, bool> fields_;

public:
	ConfigServer();
	ConfigServer(ConfigServer const &src);
	~ConfigServer();

	ConfigServer &operator=(ConfigServer const &rhs);

	bool setServAddr(sa_family_t family, std::string const &addr);
	bool setPort(std::string const &port);
	bool setServerName(std::string const &serverName);
	bool setErrorPage(std::string const &errorPage, std::vector<uint32_t> &codes);
	bool setClientBodyBufferSize(std::string const &clientBodyBufferSize);
	bool setRoot(std::string const &root);
	bool setLocations(ConfigLocation const &location);

	sockaddr_in getServAddr() const;
	in_addr_t getIpv4() const;
	uint16_t getPort() const;
	std::string const &getServerName() const;
	std::string const &getServerIp() const;
	std::string getServerIpPort() const;
	std::map<std::string, std::string> const &getErrorPage() const;
	size_t getClientBodyBufferSize() const;
	std::string const &getRoot() const;
	std::vector<ConfigLocation> const &getLocations() const;
	std::map<std::string, bool> const &getFields() const;
};

#include "ConfigLocation.hpp"

#endif