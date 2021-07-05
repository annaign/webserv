#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation()
	: prefix_(""),
	  root_(""),
	  alias_(""),
	  index_(""),
	  autoindex_(false),
	  clientBodyBufferSize_(RECV_BUFFER)
{
	this->fields_["http_methods"] = false;
	this->fields_["root"] = false;
	this->fields_["alias"] = false;
	this->fields_["index"] = false;
	this->fields_["autoindex"] = false;
	this->fields_["authBasic"] = false;
	this->fields_["authBasicUserFile"] = false;
	this->fields_["extension"] = false;
	this->fields_["scrypt"] = false;
	this->fields_["client_body_buffer_size"] = false;

	this->methods_["GET"] = true;
	this->methods_["HEAD"] = true;
	this->methods_["POST"] = false;
	this->methods_["PUT"] = false;
}

ConfigLocation::ConfigLocation(ConfigLocation const &src)
{
	*this = src;
}

ConfigLocation::~ConfigLocation()
{
}

ConfigLocation &ConfigLocation::operator=(ConfigLocation const &rhs)
{
	if (this != &rhs)
	{
		this->prefix_ = rhs.prefix_;
		this->methods_ = rhs.methods_;
		this->root_ = rhs.root_;
		this->alias_ = rhs.alias_;
		this->index_ = rhs.index_;
		this->autoindex_ = rhs.autoindex_;
		this->authBasic_ = rhs.authBasic_;
		this->authBasicUserFile_ = rhs.authBasicUserFile_;
		this->extension_ = rhs.extension_;
		this->scrypt_ = rhs.scrypt_;
		this->fields_ = rhs.fields_;
		this->methodsList_ = rhs.methodsList_;
		this->clientBodyBufferSize_ = rhs.clientBodyBufferSize_;
	}

	return *this;
}

bool ConfigLocation::setPrefix(std::string const &prefix)
{
	this->prefix_ = prefix;

	return true;
}
bool ConfigLocation::setMethods(std::vector<std::string> const &methods)
{
	if (this->fields_["http_methods"])
		return false;

	this->methods_["GET"] = false;
	this->methods_["HEAD"] = false;

	std::vector<std::string>::const_iterator method = methods.begin();

	while (method != methods.end())
	{
		if (!this->methods_.count(*method))
			return false;

		this->methods_[*method] = true;

		if (this->methodsList_.length() > 0)
			this->methodsList_ += ", ";
		this->methodsList_ += *method;

		++method;
	}
	this->fields_["http_methods"] = true;

	return true;
}

bool ConfigLocation::setRoot(std::string const &root)
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

bool ConfigLocation::setAlias(std::string const &alias)
{
	if (this->fields_["alias"])
		return false;

	this->alias_ = alias;
	this->fields_["alias"] = true;

	return true;
}

bool ConfigLocation::setIndex(std::string const &index)
{
	if (this->fields_["index"])
		return false;

	this->index_ = index;
	this->fields_["index"] = true;

	return true;
}

bool ConfigLocation::setAutoindex(std::string const &autoindex)
{
	if (this->fields_["autoindex"])
		return false;

	if (autoindex == "on")
		this->autoindex_ = true;
	else if (autoindex == "off")
		this->autoindex_ = false;
	else
		return false;

	this->fields_["autoindex"] = true;

	return true;
}

bool ConfigLocation::setAuthBasic(std::string const &file)
{
	if (this->fields_["authBasic"])
		return false;

	int fd = open(file.c_str(), O_RDONLY);
	if (fd == -1)
		return false;

	char buffer[512];
	int ret;

	while ((ret = read(fd, buffer, 512)) > 0)
		this->authBasic_.append(buffer, ret);

	if (ret == -1)
	{
		this->authBasic_ = "";
		return false;
	}

	this->fields_["authBasic"] = true;

	return true;
}

bool ConfigLocation::setAuthBasicUserFile(std::string const &file)
{
	if (this->fields_["authBasicUserFile"])
		return false;

	this->authBasicUserFile_ = file;
	this->fields_["authBasicUserFile"] = true;

	return true;
}

bool ConfigLocation::setExtension(std::string const &extension)
{
	if (this->fields_["extension"])
		return false;

	this->extension_ = extension;
	this->fields_["extension"] = true;

	return true;
}

bool ConfigLocation::setScrypt(std::string const &scrypt)
{
	if (this->fields_["scrypt"])
		return false;

	this->scrypt_ = scrypt;
	this->fields_["scrypt"] = true;

	return true;
}

bool ConfigLocation::setClientBodyBufferSize(std::string const &clientBodyBufferSize)
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

std::string const &ConfigLocation::getPrefix() const
{
	return this->prefix_;
}

std::map<std::string, bool> const &ConfigLocation::getMethods() const
{
	return this->methods_;
}

std::string const &ConfigLocation::getMethodsList() const
{
	return this->methodsList_;
}

std::string const &ConfigLocation::getRoot() const
{
	return this->root_;
}

std::string const &ConfigLocation::getAlias() const
{
	return this->alias_;
}

std::string const &ConfigLocation::getIndex() const
{
	return this->index_;
}

bool ConfigLocation::getAutoindex() const
{
	return this->autoindex_;
}

std::string ConfigLocation::getAuthBasic() const
{
	return this->authBasic_;
}

std::vector<std::string> ConfigLocation::getAuthBasicUserFile() const
{
	std::vector<std::string> users;
	std::string tmp;

	int fd = open(this->authBasicUserFile_.c_str(), O_RDONLY);
	if (fd != -1)
	{
		char buffer[512];
		int ret;

		while ((ret = read(fd, buffer, 512)) > 0)
			tmp.append(buffer, ret);

		if (ret == -1)
			tmp = "";

		users = ws_split(tmp, "\n");
	}

	return users;
}

std::string const &ConfigLocation::getExtension() const
{
	return this->extension_;
}

std::string const &ConfigLocation::getScrypt() const
{
	return this->scrypt_;
}

size_t ConfigLocation::getClientBodyBufferSize() const
{
	return this->clientBodyBufferSize_;
}

std::map<std::string, bool> const &ConfigLocation::getFields() const
{
	return this->fields_;
}
