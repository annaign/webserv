#include "ConfigServer.hpp"

ConfigServer::ConfigServer()
	: sockaddr_(),
	  serverName_(""),
	  clientBodyBufferSize_(RECV_BUFFER),
	  root_("")
{
	this->fields_["listen"] = false;
	this->fields_["server_name"] = false;
	// this->fields_["error_page"] = false;
	this->fields_["client_body_buffer_size"] = false;
	this->fields_["root"] = false;
	this->fields_["location"] = false;
}

ConfigServer::ConfigServer(ConfigServer const &src)
{
	*this = src;
}

ConfigServer::~ConfigServer()
{
}

ConfigServer &ConfigServer::operator=(ConfigServer const &rhs)
{
	if (this != &rhs)
	{
		this->sockaddr_ = rhs.sockaddr_;
		this->serverName_ = rhs.serverName_;
		this->serverIp_ = rhs.serverIp_;
		this->errorCodes_ = rhs.errorCodes_;
		this->clientBodyBufferSize_ = rhs.clientBodyBufferSize_;
		this->root_ = rhs.root_;
		this->locations_ = rhs.locations_;
		this->fields_ = rhs.fields_;
	}

	return *this;
}

bool ConfigServer::setServAddr(sa_family_t family, std::string const &addr)
{
	if (this->fields_["listen"])
		return false;

	this->sockaddr_.sin_family = family;

	std::string::size_type pos = addr.find(':');
	if (pos == std::string::npos || addr.length() == 1 || pos == addr.length() - 1)
		return false;

	this->serverIp_ = addr.substr(0, pos);

	// Change: inet_addr() => inet_aton()
	this->sockaddr_.sin_addr.s_addr = inet_addr(this->serverIp_.c_str());

	if (this->sockaddr_.sin_addr.s_addr == INADDR_NONE)
		return false;

	if (!this->setPort(addr.substr(pos + 1)))
		return false;

	this->fields_["listen"] = true;

	return true;
}

bool ConfigServer::setPort(std::string const &port)
{
	if (port.length() > 5)
		return false;

	std::string::const_iterator it = port.begin();

	while (it != port.end())
	{
		if (!std::isdigit(*it))
			return false;
		++it;
	}

	int portInt = std::atoi(port.c_str());
	if (portInt < 1 || portInt > 65535)
		return false;

	this->sockaddr_.sin_port = htons(static_cast<uint16_t>(portInt));

	return true;
}

bool ConfigServer::setServerName(std::string const &serverName)
{
	if (this->fields_["server_name"])
		return false;

	this->serverName_ = serverName;
	this->fields_["server_name"] = true;

	return true;
}

bool ConfigServer::setErrorPage(std::string const &errorPage, std::vector<uint32_t> &codes)
{
	std::vector<uint32_t>::const_iterator code = codes.begin();

	while (code != codes.end())
	{
		if (this->errorCodes_.count(positiveNumberToString(*code)))
			return false;
		this->errorCodes_[positiveNumberToString(*code)] = errorPage;
		++code;
	}

	// int fd;
	// if ((fd = open(errorPage.c_str(), O_RDONLY)) == -1)
	// 	return false;
	// close(fd);

	return true;
}

bool ConfigServer::setClientBodyBufferSize(std::string const &clientBodyBufferSize)
{
	if (this->fields_["client_body_buffer_size"])
		return false;

	std::string::const_iterator it = clientBodyBufferSize.begin();

	while (it != clientBodyBufferSize.end())
	{
		if (!std::isdigit(*it))
			return false;
		++it;
	}

	int sizeInt = std::atoi(clientBodyBufferSize.c_str());
	if (sizeInt < 1)
		return false;

	this->clientBodyBufferSize_ = static_cast<size_t>(sizeInt);
	this->fields_["client_body_buffer_size"] = true;

	return true;
}

bool ConfigServer::setRoot(std::string const &root)
{
	if (this->fields_["root"])
		return false;

	// DIR *dir = NULL;
	// if ((dir = opendir(root.c_str())) == NULL)
	// 	return false;
	// closedir(dir);

	this->root_ = root;
	this->fields_["root"] = true;

	return true;
}

bool ConfigServer::setLocations(ConfigLocation const &location)
{
	this->locations_.push_back(location);

	return true;
}

sockaddr_in ConfigServer::getServAddr() const
{
	return this->sockaddr_;
}

uint16_t ConfigServer::getPort() const
{
	return ntohs(this->sockaddr_.sin_port);
}

in_addr_t ConfigServer::getIpv4() const
{
	return this->sockaddr_.sin_addr.s_addr;
}

std::string const &ConfigServer::getServerName() const
{
	return this->serverName_;
}

std::string const &ConfigServer::getServerIp() const
{
	return this->serverIp_;
}

std::string ConfigServer::getServerIpPort() const
{
	return this->serverIp_ + ":" + positiveNumberToString(this->getPort());
}

std::map<std::string, std::string> const &ConfigServer::getErrorPage() const
{
	return this->errorCodes_;
}

size_t ConfigServer::getClientBodyBufferSize() const
{
	return this->clientBodyBufferSize_;
}

std::string const &ConfigServer::getRoot() const
{
	return this->root_;
}

std::vector<ConfigLocation> const &ConfigServer::getLocations() const
{
	return this->locations_;
}

std::map<std::string, bool> const &ConfigServer::getFields() const
{
	return this->fields_;
}
